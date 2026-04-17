CARGO = cargo

all:
	$(CARGO) build --release

run: all
	./target/release/chash

clean:
	$(CARGO) clean
	rm -f hash.log

.PHONY: all run clean