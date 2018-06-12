dot_files := $(shell ls *.dot)
svg_files := $(dot_files:.dot=.svg)

html_files := $(shell ls *.html)
pdf_files := $(html_files:.html=.pdf)

all: $(svg_files) $(pdf_files)

$(svg_files): %.svg: %.dot
	dot -Tsvg -o $@ $<

# For pdf to be valid, markdeep < 0.19 must be used...
$(pdf_files): %.pdf: %.html
	wkhtmltopdf $< $@

clean:
	rm -f $(svg_files) $(pdf_files)
