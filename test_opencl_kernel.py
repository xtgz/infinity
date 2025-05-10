import magicXorMiner
from ecdsa import SigningKey, SECP256k1

WORKSIZE_LOCAL = 64
WORKSIZE_MAX = 0  # 0 means default
INVERSE_SIZE = 255
INVERSE_MULTIPLE = 1024 



PKEY_A = "f525d3429df6961518054023cdcddb2c9ec7c1e74bda143e6a40fc28bdab76f6"
DIFF = "0000000fffffffffffffffffffffffffffffffff"

"""
    [UTILS]
    get public key on the secp256k1 elliptic curve

    private_key_hex -- hex no '0x'
"""
def get_secp256k1_pub(private_key_hex):
    sk = SigningKey.from_string(string = bytes.fromhex(private_key_hex), curve=SECP256k1)
    vk = sk.verifying_key
    public_key_bytes = vk.to_string()
    public_key_hex = public_key_bytes.hex()
    return public_key_hex

def dry_run():
    magicXorMiner.runMagicXor(
        strPublicKey = get_secp256k1_pub(PKEY_A),
        strMagicXorDifficulty = DIFF, 
        mineContract = False,
        worksizeLocal = WORKSIZE_LOCAL,
        worksizeMax = WORKSIZE_MAX,
        inverseSize = INVERSE_SIZE,
        inverseMultiple = INVERSE_MULTIPLE,
        bNoCache = False, 
        verboseStdOut = True
    )



def main():
    print("TESTING OPENCL KERNEL FOR MAGIC XOR MINING")
    print("You shall also get an idea here about OpenCL miner parameter tuning")
    print("Feel free to adjust them right in this file")
    print("Below you will see C++ & OpenCL miner output")
    print("You will also see the hash rate & initialization time")
    print()
    dry_run()


if __name__ == '__main__':
    main()