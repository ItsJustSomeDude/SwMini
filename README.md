# SwMini

SwMini is a Mod Loader, Library, and Patch Set for the mobile game Swordigo® by TouchFoo™. It adds
the ability to easily change hard-coded vanilla features, expands the Vanilla Lua libraries, improve
logging, expand functionality, and allow easier modifications.

This is accomplished through two main pieces: First, a full replacement of the Java portion of
Vanilla which handles initialization, system interfaces, and music. The second (and larger) piece is
written in C, and uses [GlossHook](<https://github.com/XMDS/GlossHook>) to modify and replace
functions from the vanilla engine, which is written in C++ and shipped as `libswordigo.so`. A large
amount of reverse-engineering, mainly using Ghidra, has been done to accomplish these changes.

SwMini runs on Android 6.0 (API Level 23) and above, the same versions that the latest version of
vanilla supports. SwMini is currently built against version 1.4.12, build 47, the latest version
as of November 2025. Effort is made to support 32-bit devices and emulators, but the primary target
is 64-bit physical devices.

The original goal of SwMini was to remove all the unnecessary code from Vanilla, and has slowly
expanded, first to allow changing certain compile-time constants, and now to fully expand and
replace engine features.

From [Wikipedia](<https://en.wikipedia.org/wiki/Swordigo>): Released in 2012, Swordigo is an
action-adventure platform game created by Finnish indie studio Touch Foo. The game is similar to
Zelda II: The Adventure of Link, Metroid, and Castlevania: Symphony of the Night. The player can
explore many areas, collect weapons, trinkets, and spells, and defeat a multitude of enemies and
powerful bosses.

## Features

### Lua Expansion

Vanilla uses Lua 5.1 as a scripting engine for all interactions, cutscenes, boss fights, and some of
the entity behaviors. Much of the Lua standard library was not included, but has been re-added in
SwMini to make modding easier and to lower the learning curve for those with pre-existing Lua
experience. These are `debug`, `io`, `math`, `os`, and `table`. They should be usable exactly the
same way as regular Lua, with a few exceptions:

- Certain functions were removed due to Android's sandboxing (`os.execute` and `io.popen`), and some
  were removed due to conflicts or crashes (`os.exit`, `os.setlocale`).
- All functions that deal with Files, Directories, and Paths were modified to use the "Virtual
  Filesystem", which simplifies working within the Android sandbox. This includes many functions
  from the `io` and `os` libraries, as well as the `loadfile` and `dofile` functions. See
  below for an explanation of how to use these.
- A couple additional third-party libraries are included, with modifications as needed.

  <!-- @formatter:off -->
  | Library       | Global Name     | Notes                                                                                |
  |---------------|-----------------|--------------------------------------------------------------------------------------|
  | LuaSocket     | `broken_socket` | Only the C portion of LuaSocket is included.                                         |
  | LuaFileSystem | `fs`            | Path translation works the same as above. `chdir` and `currentdir` are not included. |
  <!-- @formatter:on -->

- SwMini adds additional functions that interface with the engine. These are under different Tables,
  some nested under others, and some directly in the global table for legacy or simplicity. **This
  should not be considered a "fixed" API surface. Any SwMini updates may change or remove
  functions.**

  - `Mini.<function>`: This is where most of the functions live.

    <!-- @formatter:off -->
    | Function            | Arguments                 | Returns    | Description                                                                                                                                                       |
    |---------------------|---------------------------|------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------|
    | `Arch`              | `()`                      | `string`   | Returns the device architecture, "armeabi-v7a" or "arm64-v8a".                                                                                                    |
    | `GetProfileID`      | `()`                      | `string`   | Gets the UUID of the currently playing save file as a string.                                                                                                     |
    | `ExecuteLNI`        | `(function: string, ...)` | *varies*   | Runs an LNI function using it's pascalCase name.                                                                                                                  |
    | `BindLNI`           | `(function: string)`      | `function` | Returns a function that calls the given LNI function when run.                                                                                                    |
    | `SetControlsHidden` | `(hidden: boolean)`       | `nil`      | Shows or hides the controls, without enabling Cinematic Mode.                                                                                                     |
    | `RecreateHero`      | `()`                      | `nil`      | Recreates Hero, to apply Model and Weapon changes.<br />The `OnLoad` block will be re-run, and all references stored from `Scene.Find("hero")` will stop working. |
    | `SetCoinLimit`      | `(limit: number)`         | `nil`      | Sets the Soul Shard limit. Must be <= 65535, and must be re-run on every scene change.                                                                            |
    <!-- @formatter:on -->

    - `Mini.Health.<function>`: These functions are the "Get" counterparts to the vanilla's
      `Health.SetCurrentMana` function.

      <!-- @formatter:off -->
      | Function             | Arguments  | Returns  | Description                                             |
      |----------------------|------------|----------|---------------------------------------------------------|
      | `CurrentMana`        | `()`       | `number` | Gets the current number of Mana points the player has.  |
      | `CurrentManaPercent` | `()`       | `number` | Returns the percentage of the mana meter that's filled. |
      <!-- @formatter:on -->

  - `LNI.<function>`: (Lua Native Interface) These functions interface with Java to perform
    different System-level functions. Each are available in both PascalCase (Vanilla style) and
    camelCase (Java style). The parameters and return values can be `string`, `number`, or
    `boolean`, and will be passed to the underlying Java function both ways.

    <!-- @formatter:off -->
    | Function          | Arguments         | Returns  | Description                                                             |
    |-------------------|-------------------|----------|-------------------------------------------------------------------------|
    | `copyToClipboard` | `(text: string)`  | `nil`    | Copies text to the clipboard. Alias: `copy`                             |
    | `openUrl`         | `(url: string)`   | `nil`    | Opens a URL in the browser. Alias: `openURL`                            |
    | `getSpeed`        | `()`              | `number` | Gets the currently set Engine Speed as a number. 1 = Normal Speed.      |
    | `setSpeed`        | `(speed: number)` | `nil`    | Sets the Engine speed. The affects the FPS, and all physics and timers. |
    | `quit`            | `()`              | `nil`    | Exits the game.                                                         |
    <!-- @formatter:on -->

### Virtual Filesystem (MiniPaths)

SwMini adds the concept of a "Virtual Filesystem" that allows for simpler working within Android's
App Sandbox, and adds the ability to replace and modify game assets at Runtime, both for easier mod
development and runtime self-modification. This is accomplished through Path Translation applied to
all functions that interact with files in any way. Here is a list of translated paths and what they
point to:

| MiniPath          | Real Path                                | Notes                                                                                                                                                             |
|-------------------|------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `/Assets/`        | APK `/assets/` directory                 | Since this is bundled in the APK, all functions are read-only. Additionally, Symlinks and Hardlinks cannot point to `/Assets/` because they are not "real" files. |
| `/Files/`         | `/data/user/U/<pkg name>/files/`         | Use this for most private file storage. Access is Read/Write, and all File and Directory actions should work.                                                     |
| `/ExternalFiles/` | `/sdcard/Android/data/<pkg name>/files/` | This can be used as file storage. It can be accessed by some File Manager apps, so should be used to get files out if needed. All operations should work.         |
| `/Cache/`         | `/data/user/U/<pkg name>/cache/`         | All operations work on Caches, but they may be deleted by the system or user at any time.                                                                         |
| `/ExternalCache/` | `/sdcard/Android/data/<pkg name>/cache/` | Same as `/Cache/` but can be accessed by some file managers.                                                                                                      |
| `resources/`      | *Resource Search Hierarchy*              | See below.                                                                                                                                                        |

Under SwMini, when the engine is looking for resources such as Scenes, SCLs, Models, Textures, it
will try paths in this order:

1. `/ExternalFiles/resources/<profile id>/`
2. `/Files/resources/<profile id>/`
3. `/ExternalFiles/resources/`
4. `/Files/resources/`
5. `/Assets/resources/`

The profile id will be filled in with the *most recently active profile*, so after leaving to the
title screen that search path will still be active until a different file is entered or the app is
fully closed. Using the `resources/` MiniPath will perform the search the same way, so you should
not try to **write** to this path, because it may land in `/Assets/` and be read-only. (This path
base is intentionally not capitalized and does not have a leading `/` due to being the hardcoded
path the engine uses for resource loading.)

During development, mod files can be placed in
`/sdcard/Android/data/<pkg name>/files/resources/` and will take effect *at the next scene load*.
When packaging for release, place these files in the apk at `/assets/resources/`.

### Mod Properties

There are a lot of things that can be tweaked in TOML config files in the APK. Each file contains
comments explaining how they should be used. `mini.toml` includes default options, the links on the
homescreen, some info about the mod, and more. `cstring.toml` allows changing the strings the engine
uses for the UI. (Lua strings are not affected by this file.) `achievements.toml` changes the
achievements (obviously).

### Logging

All output from engine, including Lua's `print`, `io.stdout:write`, and `io.stderr:write` functions,
will be written to Android Logcat. Additionally, unprotected Lua errors will be logged. These can
then be accessed using `adb logcat` from a computer or Termux, or using an app such
as [Logcat Viewer.](<https://f-droid.org/packages/com.dp.logcatapp>)

### Custom Music

The Music now loads from the `/assets/music/` folder, and can thus be replaced or modified more
easily. Custom music tracks can also be added, and then played from Lua with
`MusicPlayer.PlayMusic()` or by setting the track as the music for a custom Scene. If you want to
adjust the default volume level for a custom track, add an entry to `options.gopt` for it (most
vanilla music is configured to play at 0.5 volume).

### Patches

A number of smaller patches are included. This includes a crash handler that will offer to export
logs on startup in case of a crash, and a patch to always execute Lua program `String`s instead of
executing `Bytes` on 32-bit devices. This makes mods work nearly the same on all devices, although
some bugs may still slip through the cracks.

### Code Cleanup

All unnecessary Java code is removed entirely. This includes the entire systems for Ads, Analytics,
Play Games, Store Purchases, and Ratings, as well as all of these library's dependencies, which add
up to HUNDREDS of classes. This makes the APK layout much cleaner, and shrinks the size by a fair
amount.

<!--
### Library Auto-loading

Compiled native libraries that are placed in the `/lib/<arch>/` folder with the extension `.so` will
be `System.loadLibrary`-ed after the builtin libraries. Note that this does not _directly_ add the
ability to add Lua libraries, and most C libraries will require modification or special care to work
inside SwMini. For Advanced usage only.
-->

## Credits

### Game

*Note that no parts of the vanilla code are directly included or distributed here. Some portions of
the Java code are based on a decompilation of the original, however no assets, music, or compiled
classes are provided.*

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

> KLib - 2008, 2009, 2011 by Attractive Chaos <attractor@live.co.uk>
>
> MIT License
>
> https://github.com/attractivechaos/klib/blob/master/khash.h

### Dependencies

*These dependencies are required for building. They are linked as Git submodules for ease of use.*

> LuaSocket - Copyright (C) 2004-2022 Diego Nehab
>
> MIT License
>
> https://github.com/lunarmodules/luasocket

> TOML C - Copyright (C) CK Tan
>
> MIT License
>
> https://github.com/arp242/toml-c

> GlossHook - Copyright (C) 2022 XMDS
>
> MIT License
>
> https://github.com/XMDS/GlossHook

*Gloss.h from this project is also embedded in the repo due to edits.*

*These dependencies are fetched from Maven.*

> JToml - Copyright (C) 2025 Xavier Pedraza
>
> Apache 2.0 License
>
> https://github.com/WasabiThumb/jtoml

## Building

**No resources, music, libraries, or pre-built code that is under TouchFoo's copyright are included
in this repo**, however these can be extracted from a vanilla apk. The APK must be the same version
that SwMini was built for (see the version at the top of the README.) This APK can be placed in the
root of the project directory, named `vanilla.apk`, or the relevant Gradle task(s) can be run with
`-Path="/path/to/vanilla/sw.apk"`, or set the Gradle Project Property `Path` to the APK location in
your IDE. Then run the Gradle task `:app:extractVanilla`. After this, you'll be ready to assemble
SwMini as any other Android App!

Example:

```shell
# Using vanilla APK placed at `vanilla.apk`
./gradlew :app:extractVanilla

# Using custom APK location
./gradlew :app:extractVanilla -Path="/path/to/sw.apk"
```

## Name?

SwMini was originally created to make vanilla smaller, but now basically has a full modding
interface. So the name might mean...

**SW**ordigo **M**odding **IN**terface by **I**jsd
