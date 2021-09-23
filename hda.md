# Intel HDA for Airbook 6,2

Notes on trying to get the HDA driver on my Airbook 6,2 working under Haiku. Linux has an HDA codec
patch for a handful of Cirrus codecs.

Target device: 0x106b/0x7200 PCI ids, 0x1013/0x4208 HDA codec ids.

    /* ops set by the preset patch */
    struct hda_codec_ops {
        int (*build_controls)(struct hda_codec *codec);
        int (*build_pcms)(struct hda_codec *codec);
        int (*init)(struct hda_codec *codec);
        void (*free)(struct hda_codec *codec);
        void (*unsol_event)(struct hda_codec *codec, unsigned int res);
        void (*set_power_state)(struct hda_codec *codec, hda_nid_t fg,
                unsigned int power_state);
        void (*reboot_notify)(struct hda_codec *codec);
        void (*stream_pm)(struct hda_codec *codec, hda_nid_t nid, bool on);
    }
    
    // snd_hda_pick_fixup
    // snd_hda_apply_fixup
    // snd_hda_override_wcaps
    // get_wcaps
    // snd_hda_override_amp_caps
    // query_amp_caps
    
    SND_PCI_QUIRK(0x106b, 0x7200, "MacBookAir 6,2", CS4208_MBA6);
    
    MBA6 => HDA_FIXUP_PINS, mba6_pincfgs
         => HDA_FIXUP_FUNC, cs4208_fixup_gpio0
    
    SND_PCI_QUIRK_VENDOR(0x106b, "Apple", CS4208_MAC_AUTO);
    
    MAC_AUTO => HDA_FIXUP_FUNC, cs4208_fixup_mac
    
    patch_cs4208?
    
    snd_hda_id_cirrus[] {
        0x10134208, "CS4208", patch_cs4208)
    }

Current Linux source: https://github.com/torvalds/linux/blob/master/sound/pci/hda/patch_cirrus.c
