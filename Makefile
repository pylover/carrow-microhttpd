PRETTYCFLAGS = --recursive \
	--verbose=0 \
	--repository=. \
	--extensions=c,h,in \
	--linelength=80 \
	--headers=h,in \
	--includeorder=standardcfirst \
	--exclude=src \
	--exclude=doc \
	--exclude=w32 \
	--exclude=po \
	--exclude=libmicrohttpd.pc.in \
	--root=. \
	./

lint:
	prettyc $(PRETTYCFLAGS)
