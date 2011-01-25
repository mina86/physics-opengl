data   := $(addprefix out/,$(addsuffix .dat,$(basename $(notdir $(wildcard results/*.bz2)))))
images := $(addprefix out/,$(addsuffix .pdf,$(basename $(notdir $(wildcard plot/*.plot)))))


all: doc.pdf

# Data file
out/%.dat: results/%.bz2
	@exec mkdir -p out
	bzip2 -d <$^ | perl filter-lines.pl 100 1000 >$@

# Images
out/%.pdf: out/%.eps
	exec epstopdf $<

out/%.eps: plot/%.plot $(data)
	@exec mkdir -p out
	gnuplot <$< >$@

# Main PDF file
doc.pdf: out/doc.pdf
	exec cp -- $< $@

out/doc.pdf:: doc.tex $(images)
	exec pdflatex -output-directory out -interaction=nonstopmode \
		-file-line-error -draftmode doc.tex
	exec pdflatex -output-directory out -interaction=nonstopmode \
		-file-line-error doc.tex

# Misc
clean::
	exec rm -rf -- out

distclean::
	exec rm -rf -- out doc.pdf

.PHONY: clean distclean
