// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * HD audio interface patch for Cirrus Logic CS420x chip
 *
 * Copyright (c) 2009 Takashi Iwai <tiwai@suse.de>
 */

#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <sound/core.h>
#include <linux/pci.h>
#include <sound/tlv.h>
#include <sound/hda_codec.h>
#include "hda_local.h"
#include "hda_auto_parser.h"
#include "hda_jack.h"
#include "hda_generic.h"

/*
 */

struct cs_spec {
	struct hda_gen_spec gen;

	unsigned int gpio_mask;
	unsigned int gpio_dir;
	unsigned int gpio_data;
	unsigned int gpio_eapd_hp; /* EAPD GPIO bit for headphones */
	unsigned int gpio_eapd_speaker; /* EAPD GPIO bit for speakers */
  
	hda_nid_t vendor_nid;
};

/* Cirrus Logic CS4208 */
#define CS4208_VENDOR_NID	0x24


/*
 * auto-mute and auto-mic switching
 * CS421x auto-output redirecting
 * HP/SPK/SPDIF
 */

static void cs_automute(struct hda_codec *codec)
{
	struct cs_spec *spec = codec->spec;

	/* mute HPs if spdif jack (SENSE_B) is present */
	spec->gen.master_mute = !!(spec->spdif_present && spec->sense_b);

	snd_hda_gen_update_outputs(codec);

	if (spec->gpio_eapd_hp || spec->gpio_eapd_speaker) {
		if (spec->gen.automute_speaker)
			spec->gpio_data = spec->gen.hp_jack_present ?
				spec->gpio_eapd_hp : spec->gpio_eapd_speaker;
		else
			spec->gpio_data =
				spec->gpio_eapd_hp | spec->gpio_eapd_speaker;
		snd_hda_codec_write(codec, 0x01, 0,
				    AC_VERB_SET_GPIO_DATA, spec->gpio_data);
	}
}

static const struct hda_verb cs4208_coef_init_verbs[] = {
	{0x01, AC_VERB_SET_POWER_STATE, 0x00}, /* AFG: D0 */
	{0x24, AC_VERB_SET_PROC_STATE, 0x01},  /* VPW: processing on */
	{0x24, AC_VERB_SET_COEF_INDEX, 0x0033},
	{0x24, AC_VERB_SET_PROC_COEF, 0x0001}, /* A1 ICS */
	{0x24, AC_VERB_SET_COEF_INDEX, 0x0034},
	{0x24, AC_VERB_SET_PROC_COEF, 0x1C01}, /* A1 Enable, A Thresh = 300mV */
	{} /* terminator */
};

static int cs_init(struct hda_codec *codec)
{
	struct cs_spec *spec = codec->spec;

	if (spec->vendor_nid == CS4208_VENDOR_NID) {
		snd_hda_sequence_write(codec, cs4208_coef_init_verbs);
	}

	snd_hda_gen_init(codec);

	if (spec->gpio_mask) {
		snd_hda_codec_write(codec, 0x01, 0, AC_VERB_SET_GPIO_MASK,
				    spec->gpio_mask);
		snd_hda_codec_write(codec, 0x01, 0, AC_VERB_SET_GPIO_DIRECTION,
				    spec->gpio_dir);
		snd_hda_codec_write(codec, 0x01, 0, AC_VERB_SET_GPIO_DATA,
				    spec->gpio_data);
	}

	return 0;
}

static int cs_build_controls(struct hda_codec *codec)
{
	int err;

	err = snd_hda_gen_build_controls(codec);
	if (err < 0)
		return err;
	snd_hda_apply_fixup(codec, HDA_FIXUP_ACT_BUILD);
	return 0;
}

#define cs_free		snd_hda_gen_free

static const struct hda_codec_ops cs_patch_ops = {
	.build_controls = cs_build_controls,
	.build_pcms = snd_hda_gen_build_pcms,
	.init = cs_init,
	.free = cs_free,
	.unsol_event = snd_hda_jack_unsol_event,
};

static int cs_parse_auto_config(struct hda_codec *codec)
{
	struct cs_spec *spec = codec->spec;
	int err;
	int i;

	err = snd_hda_parse_pin_defcfg(codec, &spec->gen.autocfg, NULL, 0);
	if (err < 0)
		return err;

	err = snd_hda_gen_parse_auto_config(codec, &spec->gen.autocfg);
	if (err < 0)
		return err;

	/* keep the ADCs powered up when it's dynamically switchable */
	if (spec->gen.dyn_adc_switch) {
		unsigned int done = 0;

		for (i = 0; i < spec->gen.input_mux.num_items; i++) {
			int idx = spec->gen.dyn_adc_idx[i];

			if (done & (1 << idx))
				continue;
			snd_hda_gen_fix_pin_power(codec,
						  spec->gen.adc_nids[idx]);
			done |= 1 << idx;
		}
	}

	return 0;
}

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

static struct cs_spec *cs_alloc_spec(struct hda_codec *codec, int vendor_nid)
{
	struct cs_spec *spec;

	spec = kzalloc(sizeof(*spec), GFP_KERNEL);
	if (!spec)
		return NULL;
	codec->spec = spec;
	spec->vendor_nid = vendor_nid;
	codec->power_save_node = 1;
	snd_hda_gen_spec_init(&spec->gen);

	return spec;
}

/*
 * CS4208 support:
 * Its layout is no longer compatible with CS4206/CS4207
 */
enum {
	CS4208_MAC_AUTO,
	CS4208_MBA6,
	CS4208_GPIO0,
};

static const struct hda_model_fixup cs4208_models[] = {
	{ .id = CS4208_GPIO0, .name = "gpio0" },
	{ .id = CS4208_MBA6, .name = "mba6" },
	{}
};

static const struct snd_pci_quirk cs4208_fixup_tbl[] = {
	SND_PCI_QUIRK_VENDOR(0x106b, "Apple", CS4208_MAC_AUTO),
	{} /* terminator */
};

/* codec SSID matching */
static const struct snd_pci_quirk cs4208_mac_fixup_tbl[] = {
	SND_PCI_QUIRK(0x106b, 0x7200, "MacBookAir 6,2", CS4208_MBA6),
	{} /* terminator */
};

static void cs4208_fixup_gpio0(struct hda_codec *codec,
			       const struct hda_fixup *fix, int action)
{
	if (action == HDA_FIXUP_ACT_PRE_PROBE) {
		struct cs_spec *spec = codec->spec;

		spec->gpio_eapd_hp = 0;
		spec->gpio_eapd_speaker = 1;
		spec->gpio_mask = spec->gpio_dir =
			spec->gpio_eapd_hp | spec->gpio_eapd_speaker;
	}
}

static const struct hda_fixup cs4208_fixups[];

/* remap the fixup from codec SSID and apply it */
static void cs4208_fixup_mac(struct hda_codec *codec,
			     const struct hda_fixup *fix, int action)
{
	if (action != HDA_FIXUP_ACT_PRE_PROBE)
		return;

	codec->fixup_id = HDA_FIXUP_ID_NOT_SET;
	snd_hda_pick_fixup(codec, NULL, cs4208_mac_fixup_tbl, cs4208_fixups);
	if (codec->fixup_id == HDA_FIXUP_ID_NOT_SET)
		codec->fixup_id = CS4208_GPIO0; /* default fixup */
	snd_hda_apply_fixup(codec, action);
}

static const struct hda_fixup cs4208_fixups[] = {
	[CS4208_MBA6] = {
		.type = HDA_FIXUP_PINS,
		.v.pins = mba6_pincfgs,
		.chained = true,
		.chain_id = CS4208_GPIO0,
	},
	[CS4208_GPIO0] = {
		.type = HDA_FIXUP_FUNC,
		.v.func = cs4208_fixup_gpio0,
	},
	[CS4208_MAC_AUTO] = {
		.type = HDA_FIXUP_FUNC,
		.v.func = cs4208_fixup_mac,
	},
};

/* correct the 0dB offset of input pins */
static void cs4208_fix_amp_caps(struct hda_codec *codec, hda_nid_t adc)
{
	unsigned int caps;

	caps = query_amp_caps(codec, adc, HDA_INPUT);
	caps &= ~(AC_AMPCAP_OFFSET);
	caps |= 0x02;
	snd_hda_override_amp_caps(codec, adc, HDA_INPUT, caps);
}

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

	snd_hda_pick_fixup(codec, cs4208_models, cs4208_fixup_tbl,
			   cs4208_fixups);
	snd_hda_apply_fixup(codec, HDA_FIXUP_ACT_PRE_PROBE);

	snd_hda_override_wcaps(codec, 0x18,
			       get_wcaps(codec, 0x18) | AC_WCAP_STEREO);
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

/*
 * patch entries
 */
static const struct hda_device_id snd_hda_id_cirrus[] = {
	HDA_CODEC_ENTRY(0x10134208, "CS4208", patch_cs4208),
	{} /* terminator */
};
MODULE_DEVICE_TABLE(hdaudio, snd_hda_id_cirrus);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Cirrus Logic HD-audio codec");

static struct hda_codec_driver cirrus_driver = {
	.id = snd_hda_id_cirrus,
};

module_hda_codec_driver(cirrus_driver);
