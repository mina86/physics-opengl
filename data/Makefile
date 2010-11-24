CONVERT			?= convert
DATA_DIR		?= .
JPG_TEXTURES	:= $(wildcard *.jpg)
TEXTURES_		 = $(addprefix $(DATA_DIR)/,$(basename $(JPG_TEXTURES)))
SRC_FILES		:= $(wildcard *.sol) $(wildcard *.psy) README


all: textures files

textures: hq mq lq

hq: $(addsuffix .hq.sgi,$(TEXTURES_))
mq: $(addsuffix .mq.sgi,$(TEXTURES_))
lq: $(addsuffix .lq.sgi,$(TEXTURES_))

files: $(addprefix $(DATA_DIR)/,$(SRC_FILES))


%.hq.sgi: %.jpg
	$(CONVERT) $< -resize 1024x512\! $@

%.mq.sgi: %.jpg
	$(CONVERT) $< -resize 512x256\! $@

%.lq.sgi: %.jpg
	$(CONVERT) $< -resize 256x128\! $@


$(DATA_DIR)/%.hq.sgi: %.jpg
	$(CONVERT) $< -resize 1024x512\! $@

$(DATA_DIR)/%.mq.sgi: %.jpg
	$(CONVERT) $< -resize 512x256\! $@

$(DATA_DIR)/%.lq.sgi: %.jpg
	$(CONVERT) $< -resize 256x128\! $@


$(DATA_DIR)/%: %
	exec cp -- $< $@



clean:
	exec rm -- *.sgi

distclean: clean
