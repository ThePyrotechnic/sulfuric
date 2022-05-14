include make.env

.PHONY: copy-vst3

copy-vst:
	cp Builds/sulfuric_artefacts/VST3/sulfuric.vst3/Contents/x86_64-linux/sulfuric.so $(VST3_DIR)
