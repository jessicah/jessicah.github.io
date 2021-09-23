# Intel HDA for Airbook 6,2

Notes on trying to get the HDA driver on my Airbook 6,2 working under Haiku. Linux has an HDA codec
patch for a handful of Cirrus codecs.

Target device: PCI `0x106b:0x7200`, HDA codec: `0x1013:0x4208`.

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

[Current Linux source](https://github.com/torvalds/linux/blob/master/sound/pci/hda/patch_cirrus.c)

## Pieces of Interest

### Patch CS4208

    static int patch_cs4208(struct hda_codec *codec)
    {
        struct cs_spec *spec;
        int err;
        
        spec = cs_alloc_spec(codec, CS4208_VENDOR_NID);
        if (!spec)
            return -ENOMEM;
        
        codec->patch_ops = cs_patch_ops;
        spec->gen.automute_hook = cs_automute;
        /* exclude NID 0x10 (HP) from output volumes due to different steps */
        spec->gen.out_vol_mask = 1ULL << 0x10;
        
        snd_hda_pick_fixup(codec, cs4208_models, cs4208_fixup_tbl, cs4208_fixups);
        snd_hda_apply_fixup(codec, HDA_FIXUP_ACT_PRE_PROBE);
        
        snd_hda_override_wcaps(codec, 0x18, get_wcaps(codec, 0x18) | AC_WCAP_STEREO);
        cs4208_fix_amp_caps(codec, 0x18);
        cs4208_fix_amp_caps(codec, 0x1b);
        cs4208_fix_amp_caps(codec, 0x1c);
        
        err = cs_parse_auto_config(codec);
        if (err < 0)
            goto error;
        
        snd_hda_apply_fixup(codec, HDA_FIXUP_ACT_PROBE);
        
        return 0;
        
    error:
        cs_free(codec);
        return err;
    }

* cs_alloc_spec
* cs_patch_ops
* cs_automute
* cs4208_models
* cs4208_fixup_tbl
* cs_parse_auto_config
* cs_free

### CS4208 Fix Amp Caps

    /* correct the 0db offset of input pins */
    static void cs4208_fix_amp_caps(struct hda_codec *codec, hda_nid_t adc)
    {
        unsigned int caps;
        
        caps = query_amp_caps(codec, adc, HDA_INPUT);
        caps &= ~(AC_AMPCAP_OFFSET);
        caps |= 0x02;
        snd_hda_override_amp_caps(codec, adc, HDA_INPUT, caps);
    }

### CS4208 Fixups

    hda_fixup cs4208_fixups[] = {
        [CS4208_MBA6] = {
            .type     = HDA_FIXUP_PINS,
            .v.pins   = mba6_pincfgs,
            .chained  = true,
            .chain_id = CS4208_GPIO0
        },
        [CS2408_GPIO0] = {
            .type     = HDA_FIXUP_FUNC,
            .v.func   = cs4208_fixup_gpio0
        },
        [CS4208_MAC_AUTO] = {
            .type     = HDA_FIXUP_FUNC,
            .v.func   = cs4208_fixup_mac
        }
    }

### CS4208 GPIO0 Fixup

    static void cs4208_fixup_gpio0(struct hda_codec *codec, const struct hda_fixup *fix, int action)
    {
        if (action == HDA_FIXUP_ACT_PRE_PROBE) {
            struct cs_spec *spec = codec->spec;
            
            spec->gpio_eapd_hp = 0;
            spec->gpio_eapd_speaker = 1;
            spec->gpio_mask = spec->gpio_dir = spec->gpio_eapd_hp | spec->gpio_eapd_speaker;
        }
    }

### CS4208 Pin Configs

    static const struct hda_pintbl mba6_pincfgs[] = {
        { 0x10, 0x032120f0 }, /* HP */
        { 0x11, 0x500000f0 },
        { 0x12, 0x90100010 }, /* Speaker */
        { 0x13, 0x500000f0 },
        { 0x14, 0x500000f0 },
        { 0x15, 0x770000f0 },
        { 0x16, 0x770000f0 },
        { 0x17, 0x430000f0 },
        { 0x18, 0x43ab9030 }, /* Mic */
        { 0x19, 0x770000f0 },
        { 0x1a, 0x770000f0 },
        { 0x1b, 0x770000f0 },
        { 0x1c, 0x90a00090 },
        { 0x1d, 0x500000f0 },
        { 0x1e, 0x500000f0 },
        { 0x1f, 0x500000f0 },
        { 0x20, 0x500000f0 },
        { 0x21, 0x430000f0 },
        { 0x22, 0x430000f0 },
        {} /* terminator */
    };

    static const struct snd_pci_quirk cs4208_fixup_tbl[] = {
        SND_PCI_QUIRK_VENDOR(0x106b, "Apple", CS4208_MAC_AUTO),
        {} /* terminator */
    };

### CS4208 Fixup Mac

    /* remap the fixup from codec SSID and apply it */
    static void cs4208_fixup_mac(struct hda_codec *codec, const struct hda_fixup *fix, int action)
    {
        if (action != HDA_FIXUP_ACT_PRE_PROBE)
            return;

        codec->fixup_id = HDA_FIXUP_ID_NOT_SET;
        snd_hda_pick_fixup(codec, NULL, cs4208_mac_fixup_tbl, cs4208_fixups);
        if (codec->fixup_id == HDA_FIXUP_ID_NOT_SET)
            codec->fixup_id = CS4208_GPIO0; /* default fixup */
        snd_hda_apply_fixup(codec, action);
    }
