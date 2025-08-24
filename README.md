# SwMini

The goal of SwMini is to remove all unnecessary code from Vanilla, and to extend modding capabilities where possible. This includes re-adding the missing Lua libraries, the ability to modify C strings (like menus and achievements), customize links, and additional engine functions in Lua.

SwMini is built from vanilla version 1.4.9, build 43. Effort is made to support 32-bit devices and emulators, but the primary target is 64-bit physical devices.

## Features

#### Code Cleanup
All unnecessary Java code is removed entirely. This includes the entire systems for Ads, Analytics, Play Games, Store Purchases, and Ratings, as well as all of these library's dependencies, which add up to HUNDREDS of classes. This makes the APK layout much cleaner, and shrinks the size by a fair amount.

#### Custom Music
The Music now loads from the `/assets/music/` folder, and can thus be replaced or modified more easily. Custom music tracks can also be added, and then played from Lua with `MusicPlayer.PlayMusic()` or by setting the track as the music for a custom Scene. If you want to adjust the default volume level for a custom track, add an entry to `options.gopt` for it (most vanilla music is configured to play at 0.5 volume).

#### Custom Links
The links on the main menu can be customized in the `/assets/mini.properties` file. See comments in the file for instructions.

#### Engine Patches
A patch is applied to make "String" Lua chunks apply to 32-bit devices. However, for this to work, a "Bytes" chunk must be *included*, but can simply have a single character.

#### Lua Libraries
All the regular Lua Libraries that are missing from Vanilla are re-added. These are `debug`, `io`, `math`, `os`, and `table`. They should be usable exactly the same way as regular Lua, with a few exceptions:

Functions that allow for arbitrary shell functions are removed due to Android's sandboxing. This includes `os.execute` and `io.popen`. Additionally, functions that are likely to cause engine problems were also removed, including `os.exit` and `os.setlocale`.

Paths must be translated slightly when using `io` and `os` functions due to Android's file restrictions. These restrictions also mean that raw access to `/sdcard/` is not possible, at least without granting overly broad permissions (which is not planned for SwMini).

`/ExternalFiles/hello.txt` -> `/sdcard/Android/<package>/files/hello.txt`
`/ExternalCache/hello.txt` -> `/sdcard/Android/<package>/cache/hello.txt`
`/Files/hello.txt` -> `/data/data/<package>/files/hello.txt`
`/Cache/hello.txt` -> `/data/data/<package>/cache/hello.txt`

Note that Cache directories may be emptied by the user by clearing the cache from App Info, or automatically by the system under certain circumstances. `/ExternalFiles/` may be accessed by some File Manager apps, so should be used to get files to outside if needed.

Additionally, the following third-party libraries are included:

| Library       | Global Name     | Notes                                                                                |
|---------------|-----------------|--------------------------------------------------------------------------------------|
| LuaSocket     | `broken_socket` | Only the C portion of LuaSocket is included.                                         |
| LuaFileSystem | `fs`            | Path translation works the same as above. `chdir` and `currentdir` are not included. |

#### Lua Extensions

SwMini provides some Lua functions that interface with the engine in ways that regular Lua is not able to. Note that these are currently all just placed into a global table named `Mini`, but may be moved at any point. Therefore, **These APIs should not be considered stable, and may change with any SwMini release.**

| Function          | Arguments         | Returns  | 32bit | 64bit | Description                                                       |
|-------------------|-------------------|----------|:-----:|:-----:|-------------------------------------------------------------------|
| Arch              | ()                | `string` |   ✓   |   ✓   | Returns the device architecture, "armeabi-v7a" or "arm64-v8a"     |
| GetProfileID      | ()                | `string` |   ✓   |   ✓   | Gets the UUID of the currently playing save file as a string.     |
| SetControlsHidden | (hidden: boolean) | `nil`    |   ✗   |   ✓   | Shows or hides all the controls, without enabling Cinematic Mode. |
| ExecuteLNI        | (...)             | *any*    |   ✓   |   ✓   | Executes a Registered Java function, see below.                   |


Usage example:

```lua
-- Hide controls, without enabling cinematic mode.
if (Mini.Arch() == "arm64-v8a") then
    Mini.SetControlsHidden(true)
else
    Game.SetCinematicMode(true, false)
end
```

Additional engine functions can be researched and potentially added if feasible. Open a GitHub Discussion to make a request.

#### C String Replacement
SwMini provides a way to replace strings that are normally hard-coded in the engine's compiled C code. This allows for translations, customizing Achievements and menus, and more. This does not affect Lua strings, which can be modified with traditional modding methods. Read the comments in the `/assets/cstrings.properties` file to learn more.

#### Mod Properties
Use the `/assets/mini.properties` file to put information about your mod, edit link targets, change engine speed, pause on lost focus, and more. Read the comments in the file for more information.

#### LNI

*Note: LNI was created before SwMini was able to properly extend Lua, so it uses a bridge using the Lua print statement. The eventual goal is to migrate this to use proper Lua C functions, but this is here for now.*

SwMini provides a few functions that can be called from Lua by way of the `print` function. Allowed types for arguments are String and Number only. The available functions are:

```lua
copyToClipboard(content: string);
copyToClipboard(title: string, content: string);
-- Alias: copy

openURL(url: string);

setSpeed(speed: number);
quit();
```

Included is a Lua library to help perform LNI calls. This should be copied from `assets/mini_resources/lni.scl` into your `FileRift/re_in` folder and added as an `ImportedLibrary`, either in the scenes where you need it, or in `hiro.scl` to access it everywhere. Once imported, it can be used by simply calling `LNI.copyToClipboard("Hello, world!")` for example.

If building the calls yourself instead of using the Lua Library, the format of a call is:

```lua
print("\127LNI:functionName(<arguments>);")

-- Be careful to include two layers of Backslash Escaping. Example:
print("\127LNI:copyToClipboard(\"Hello\\nfrom\\nLNI!\");")
-- The included Lua Library will do this for you.
```

#### Logs
Log messages from the engine, including Lua `print` messages and `io.stderr:write`, are redirected to Android logcat, which can then be accessed using `adb` or an app such as [Logcat Viewer.](<https://f-droid.org/packages/com.dp.logcatapp>)

#### Library Auto-loading
Compiled native libraries that are placed in the `/lib/<arch>/` folder with the extension `.so` will be `System.loadLibrary`'ed after the builtin libraries. Note that this does not _directly_ add the ability to add Lua libraries, and most C libraries will require modification or special care to work inside SwMini. For Advanced usage only.

## Credits

### Game

*Note that no parts of the vanilla code are directly included or distributed here. Some portions of the Java code are based on a decompilation of the original, however no assets, music, or compiled classes are provided.*

> Swordigo - Copyright (C) 2013 Ville Mäkynen.
> 
> All Rights Reserved
> 
> http://www.touchfoo.com/swordigo

### Embedded code

*The SwMini Git repo directly includes code from these open-source projects:*

> Lua 5.1 - Copyright (C) 1994-2012 Lua.org, PUC-Rio.
>
> MIT License
>
> https://www.lua.org/versions.html#5.1

> LuaFileSystem
> Copyright (C) 2003-2010 Kepler Project.
> Copyright (C) 2010-2022 The LuaFileSystem authors.
>
> MIT License
>
> https://github.com/lunarmodules/luafilesystem

### Dependencies

*These dependencies are required for building. They are linked as Git submodules for ease of use.*

> LuaSocket - Copyright (C) 2004-2022 Diego Nehab
> 
> MIT License
> 
> https://github.com/lunarmodules/luasocket

> GlossHook - Copyright (c) 2022 XMDS
> 
> MIT License
> 
> https://github.com/XMDS/GlossHook
*Gloss.h from this project is also embedded in the repo due to edits.*


## Building

**No resources, music, libraries, or pre-built code that is under TouchFoo's copyright are included in this repo**, however these can be extracted from a vanilla apk. The APK must be the same version that SwMini was built for (see the version at the top of the README.) This APK can be placed in the root of the project directory, named `vanilla.apk`, or the relevant Gradle task(s) can be run with `-Path="/path/to/vanilla/sw.apk"`, or set the Gradle Project Property `Path` to the APK location in your IDE. Then run the gradle task `:app:extractVanilla`. After this, you'll be ready to assemble SwMini as any other Android App!

Example:
```shell
# Using vanilla APK placed at `vanilla.apk`
./gradlew :app:extractVanilla

# Using custom APK location
./gradlew :app:extractVanilla -Path="/path/to/sw.apk"
```

## Name?

SwMini was originally created to make vanilla smaller, but now basically has a full modding interface. So the name could mean...

**SW**ordigo **M**odding **IN**terface by **I**jsd