## Things you need 
###

1. Xcode command line tools for mac

    or

    MSVC build tools for PC

2. nodejs & npm

3. cmake-js

4. yarn

## Windows build instructions

1. first install all node modules.

        npm install

2. use yarn to download electron-builder

        yarn add electron-builder --dev
        
3. add code signing to /electron-builder.env

    "CSC_LINK=" The HTTPS link (or base64-encoded data, or file:// link, or local path) to certificate (*.p12 or *.pfx file). Shorthand ~/ is supported (home directory).

    "#CSC_KEY_PASSWORD=" The password to decrypt the certificate given in CSC_LINK. (if applies)
    
4. use yarn to build installer
        
        yarn dist
        
        
## Mac OS build instructions
* the included .node file under /build/release is for Windows

1. install Xcode CLI tools

        xcode-select --install

2. install all node modules first

        npm install

3. install cmake-js globally

        npm install cmake-js -g

4. run cmake-js

        cmake-js rebuild
        
5. copy the icon.icns file under /lib/img/ to /build/release
    this is the icon file
    
6. use yarn to download electron-builder

        yarn add electron-builder --dev
        
7. use yarn to build installer
        
        yarn dist
        
## Linux build
*tested on ubuntu 18.04

1. Install udev library