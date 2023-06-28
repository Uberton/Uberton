# Building macOS installers

[Packages](http://s.sudre.free.fr/Software/Packages/about.html) 1.2.9+ is required for building macOS installers.

Supplied Packages projects expect the plugin binaries to be found at:
`../../../../vst3sdk/build/VST3/Release/` (relative to the .pkgproj file)

When generating and building the Uberton project in Release configuration on macOS without using Xcode, the installer will be built automatically in `Uberton/build/Installers/`
