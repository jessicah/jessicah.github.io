Notes about Rust CPAL
=====================

Just trying to get an idea of how one would potentially implement the Rust library, `cpal`, on Haiku.

`mod.rs`:
```
pub mod mediakit_import;

// Bindings import
use self::mediakit_import as mk;

/// A handle to the MediaKit API.
#[derive(Debug)]
pub struct MediaKit {

}

#[derive(Debug, FromPrimitive)]
#[repr(C)]
pub enum MedaKitSampleType {

}
```

VLC References
--------------

- `BSoundPlayer`
- `B_AUDIO_SHORT`
- `B_MEDIA_HOST_ENDIAN`
- `BSoundPlayer::BSoundPlayer` -- takes a callback, example is called PlayBuffer
- `BSoundPlayer::InitCheck`
- `BSoundPlayer::SetHasData`
- `BSoundPlayer::Start`
- `BSoundPlayer::Stop`
- `BSoundPlayer::SetVolumeDB`
- `BSoundPlayer::SetVolume`

`static void PlayBuffer(void *cookie, void *buffer, size_t size, const media_raw_audio_format &format);`

SDL References
--------------

Implementation is pretty much the same use of the `BSoundPlayer` API as VLC does.
