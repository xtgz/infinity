# `Element.matches`

[GitHub](https://github.com/jelmerdemaat/element-matches) | [NPM](https://www.npmjs.com/package/element-matches) | [@jelmerdemaat](https://twitter.com/jelmerdemaat)

Super small `Element.matches` fix for older browsers. The only thing this package does is add prefixes for:

* old IE versions (9+)
* IE mobile(10+)
* old Edge versions (12+)
* Safari (desktop) versions 5 - 7
* Safari & Chrome for iOS 4.1 - 7.1
* a number of Android browsers
    * Android 2.2 - 4.4.4
    * UC Browser 11.4
    * Blackberry Browser 7+
    * Samsung Internet 4

See Can I Use data: [https://www.caniuse.com/#feat=matchesselector](https://www.caniuse.com/#feat=matchesselector)

## Usage
Install it and import it in your code:

```sh
npm install --save element-matches
```

```js
import 'element-matches';

// Do some matching!
if (document.body.matches('.cool-body')) {
    console.info('Yeay, I am cool!');
}
```
