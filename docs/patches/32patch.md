# patches/32patch

This patch makes the `Program::String` block execute on 32bit devices to match the functionality of
64bit devices. You may remove the Bytes block completely.

There are no settings for this patch.

##### Implimentation notes:
---
Patch is ommited by Macros on 64bit devices.

Source File: `cpp/patches/32patch.c`
Header is shared with other patches, `cpp/patches/patches.h`

###### Hooks
Function: `Caver::Proto::Program::MergePartialFromCodedStream(google::protobuf::io::CodedInputStream*)`
Symbol: `_ZN5Caver5Proto7Program27MergePartialFromCodedStreamEPN6google8protobuf2io16CodedInputStreamE`

* Calls original
* Decodes "presence flags" for three strings in resulting structure.
* Early returns if "string" block is missing or empty.
* If bytes and string are present, switch them.
* If only string is present, place it in bytes, and place the default string in its place.
* Rebuilds presence flags to match new states.

