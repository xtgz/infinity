#include "profanity.cpp"

// runMagicXor() – a function that directly takes all necessary inputs and, after running the custom “magicxor” kernel (and all other necessary
// steps based on the logic in profanity.cpp::main()), returns the magicxor result string (which Dispatcher::getMagicXorFound() already provides).
//
// Parameters:
//   publicKey         : The seed public key (a 128-character hexadecimal string)
//   magicxorDifficulty: A string representing the difficulty parameter for the custom magicxor mode
//   mineContract      : (Optional) if true, use the CONTRACT target; otherwise, use ADDRESS (default: false)
//   worksizeLocal     : (Optional) OpenCL local work size (default: 64)
//   worksizeMax       : (Optional) OpenCL global work size limit; if 0 it is automatically determined (default: 0)
//   inverseSize       : (Optional) Number of modular inversions to compute per work item (default: 255)
//   inverseMultiple   : (Optional) Number of work items in parallel (default: 16384)
//   noCache           : (Optional) disable binary caching (default: false)
//   deviceSkipIndices : (Optional) indices of devices to skip (default: empty vector)
// TODO: update to skip some text output
std::string runMagicXor(
    const std::string & strPublicKey,
    const std::string & strMagicXorDifficulty,
    bool mineContract = false,
    size_t worksizeLocal = 64,
    size_t worksizeMax = 0,
    size_t inverseSize = 255,
    size_t inverseMultiple = 1024,
    bool bNoCache = false,
    const std::vector<size_t> & vDeviceSkipIndex = std::vector<size_t>(),
    bool verboseStdOut = false
){
    const std::string retFailStatus = "FAIL";

	try {
		Mode mode = Mode::magicXor(strMagicXorDifficulty);
        mode.target = ADDRESS;

		if (strPublicKey.length() == 0) {
            if (verboseStdOut) {
                std::cout << "error: this tool requires your public key to derive it's private key security" << std::endl;
            }
			return retFailStatus;
		}

		if (strPublicKey.length() != 128) {
            if (verboseStdOut) {
			    std::cout << "error: public key must be 128 hexademical characters long" << std::endl;
            }
			return retFailStatus;
		}

		std::vector<cl_device_id> vFoundDevices = getAllDevices();
		std::vector<cl_device_id> vDevices;
		std::map<cl_device_id, size_t> mDeviceIndex;

		std::vector<std::string> vDeviceBinary;
		std::vector<size_t> vDeviceBinarySize;
		cl_int errorCode;
		bool bUsedCache = false;

        if (verboseStdOut) {
		    std::cout << "Devices:" << std::endl;
        }
		for (size_t i = 0; i < vFoundDevices.size(); ++i) {
			// Ignore devices in skip index
			if (std::find(vDeviceSkipIndex.begin(), vDeviceSkipIndex.end(), i) != vDeviceSkipIndex.end()) {
				continue;
			}

			cl_device_id & deviceId = vFoundDevices[i];

			const auto strName = clGetWrapperString(clGetDeviceInfo, deviceId, CL_DEVICE_NAME);
			const auto computeUnits = clGetWrapper<cl_uint>(clGetDeviceInfo, deviceId, CL_DEVICE_MAX_COMPUTE_UNITS);
			const auto globalMemSize = clGetWrapper<cl_ulong>(clGetDeviceInfo, deviceId, CL_DEVICE_GLOBAL_MEM_SIZE);
			bool precompiled = false;

			// Check if there's a prebuilt binary for this device and load it
			if(!bNoCache) {
				std::ifstream fileIn(getDeviceCacheFilename(deviceId, inverseSize), std::ios::binary);
				if (fileIn.is_open()) {
					vDeviceBinary.push_back(std::string((std::istreambuf_iterator<char>(fileIn)), std::istreambuf_iterator<char>()));
					vDeviceBinarySize.push_back(vDeviceBinary.back().size());
					precompiled = true;
				}
			}
            if (verboseStdOut) {
			    std::cout << "  GPU" << i << ": " << strName << ", " << globalMemSize << " bytes available, " << computeUnits << " compute units (precompiled = " << (precompiled ? "yes" : "no") << ")" << std::endl;
            }
			vDevices.push_back(vFoundDevices[i]);
			mDeviceIndex[vFoundDevices[i]] = i;
		}

		if (vDevices.empty()) {
			return retFailStatus;
		}

        if (verboseStdOut) {
            std::cout << std::endl;
            std::cout << "Initializing OpenCL..." << std::endl;
            std::cout << "  Creating context..." << std::flush;
        }
		auto clContext = clCreateContext( NULL, vDevices.size(), vDevices.data(), NULL, NULL, &errorCode);
		if (printResult(clContext, errorCode, verboseStdOut)) {
			return retFailStatus;
		}

		cl_program clProgram;
		if (vDeviceBinary.size() == vDevices.size()) {
			// Create program from binaries
			bUsedCache = true;

            if (verboseStdOut) {
			    std::cout << "  Loading kernel from binary..." << std::flush;
            }
			const unsigned char * * pKernels = new const unsigned char *[vDevices.size()];
			for (size_t i = 0; i < vDeviceBinary.size(); ++i) {
				pKernels[i] = reinterpret_cast<const unsigned char *>(vDeviceBinary[i].data());
			}

			cl_int * pStatus = new cl_int[vDevices.size()];

			clProgram = clCreateProgramWithBinary(clContext, vDevices.size(), vDevices.data(), vDeviceBinarySize.data(), pKernels, pStatus, &errorCode);
			if(printResult(clProgram, errorCode, verboseStdOut)) {
				return retFailStatus;
			}
		} else {
			// Create a program from the kernel source
            if (verboseStdOut) {
			    std::cout << "  Compiling kernel..." << std::flush;
            }
			const std::string strKeccak = readFile("keccak.cl");
			const std::string strVanity = readFile("profanity.cl");
			const char * szKernels[] = { strKeccak.c_str(), strVanity.c_str() };

			clProgram = clCreateProgramWithSource(clContext, sizeof(szKernels) / sizeof(char *), szKernels, NULL, &errorCode);
			if (printResult(clProgram, errorCode, verboseStdOut)) {
				return retFailStatus;
			}
		}

		// Build the program
        if (verboseStdOut) {
		    std::cout << "  Building program..." << std::flush;
        }
		const std::string strBuildOptions = "-D PROFANITY_INVERSE_SIZE=" + toString(inverseSize) + " -D PROFANITY_MAX_SCORE=" + toString(PROFANITY_MAX_SCORE);
		if (printResult(clBuildProgram(clProgram, vDevices.size(), vDevices.data(), strBuildOptions.c_str(), NULL, NULL), verboseStdOut)) {
			return retFailStatus;
		}

		// Save binary to improve future start times
		if( !bUsedCache && !bNoCache ) {
            if (verboseStdOut) {
			    std::cout << "  Saving program..." << std::flush;
            }
			auto binaries = getBinaries(clProgram);
			for (size_t i = 0; i < binaries.size(); ++i) {
				std::ofstream fileOut(getDeviceCacheFilename(vDevices[i], inverseSize), std::ios::binary);
				fileOut.write(binaries[i].data(), binaries[i].size());
			}
            if (verboseStdOut) {
			    std::cout << "OK" << std::endl;
            }
		}
        if (verboseStdOut) {
		    std::cout << std::endl;
        }

		Dispatcher d(clContext, clProgram, mode, worksizeMax == 0 ? inverseSize * inverseMultiple : worksizeMax, inverseSize, inverseMultiple, 0, strPublicKey);
		for (auto & i : vDevices) {
			d.addDevice(i, worksizeLocal, mDeviceIndex[i]);
		}
        d.verboseStdOut = verboseStdOut;
		d.run();
		clReleaseContext(clContext);

        std::string m_magicxorFound = d.getMagicxorFound();
		return m_magicxorFound;

	} catch (std::runtime_error & e) {
        if (verboseStdOut) {
		    std::cout << "std::runtime_error - " << e.what() << std::endl;
        }
	} catch (...) {
        if (verboseStdOut) {
		    std::cout << "unknown exception occured" << std::endl;
        }
	}

	return retFailStatus;
}

