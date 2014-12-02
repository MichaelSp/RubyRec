#include <ruby.h>
#include "extconf.h"

VALUE record;
#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>

struct capture_context {
	snd_pcm_t *handle;
	snd_pcm_hw_params_t *hw_params;
};

VALUE record_allocate(VALUE self) {
	struct capture_context *capture;
  return Data_Make_Struct(self, struct capture_context, NULL, RUBY_DEFAULT_FREE, capture);
}

VALUE record_initialize(VALUE self, VALUE capture_device){
	int err;
	unsigned int sample_rate = 44100;
	struct capture_context *capture;
	Data_Get_Struct(self, struct capture_context, capture);

	rb_iv_set(self, "@device", capture_device);

	if ((err = snd_pcm_open (&capture->handle, StringValueCStr(capture_device), SND_PCM_STREAM_CAPTURE, 0)) < 0) {
		rb_raise(rb_eRuntimeError, "Cannot open audio device %s (%s)", StringValueCStr(capture_device), snd_strerror(err));
		return T_NIL;
	}

	if ((err = snd_pcm_hw_params_malloc (&capture->hw_params)) < 0) {
		rb_raise(rb_eRuntimeError, "Cannot allocate hardware parameter structure (%s)", snd_strerror (err));
		return T_NIL;
	}

	if ((err = snd_pcm_hw_params_any (capture->handle, capture->hw_params)) < 0) {
		rb_raise(rb_eRuntimeError, "Cannot initialize hardware parameter structure (%s)", snd_strerror (err));
		return T_NIL;
	}

	if ((err = snd_pcm_hw_params_set_access (capture->handle, capture->hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		rb_raise(rb_eRuntimeError, "Cannot set access type (%s)", snd_strerror (err));
		return T_NIL;
	}

	if ((err = snd_pcm_hw_params_set_format (capture->handle, capture->hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
		rb_raise(rb_eRuntimeError, "Cannot set sample format (%s)", snd_strerror (err));
		return T_NIL;
	}

	if ((err = snd_pcm_hw_params_set_rate_near (capture->handle, capture->hw_params, &sample_rate, 0)) < 0) {
		rb_raise(rb_eRuntimeError, "Cannot set sample rate (%s)", snd_strerror (err));
		return T_NIL;
	}

	if ((err = snd_pcm_hw_params_set_channels (capture->handle, capture->hw_params, 2)) < 0) {
		rb_raise(rb_eRuntimeError, "Cannot set channel count (%s)", snd_strerror (err));
		return T_NIL;
	}

	if ((err = snd_pcm_hw_params (capture->handle, capture->hw_params)) < 0) {
		rb_raise(rb_eRuntimeError, "Cannot set parameters (%s)", snd_strerror (err));
		return T_NIL;
	}

	snd_pcm_hw_params_free (capture->hw_params);

	if ((err = snd_pcm_prepare (capture->handle)) < 0) {
		rb_raise(rb_eRuntimeError, "Cannot prepare audio interface for use (%s)", snd_strerror (err));
		return T_NIL;
	}

	return self;
}

VALUE record_capture(VALUE self)
{
	int i;
	int err;
	short buf[128];
	struct capture_context *capture;
	Data_Get_Struct(self, struct capture_context, capture);

	for (i = 0; i < 10; ++i) {
		if ((err = snd_pcm_readi (capture->handle, buf, 128)) != 128) {
			rb_raise(rb_eRuntimeError, "read from audio interface failed (%s)", snd_strerror (err));
			return self;
		}
	}

	snd_pcm_close (capture->handle);
	return self;
}


void Init_record()
{
  record = rb_define_class("Record", rb_cObject);
  rb_define_alloc_func(record, record_allocate);
  rb_define_attr(record, "device", 1, 0);
  rb_define_method(record, "initialize", record_initialize, 1);
  rb_define_method(record, "capture", record_capture, 0);
}