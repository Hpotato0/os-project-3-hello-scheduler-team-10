COMP = aarch64-linux-gnu-gcc
FLAGS = -static
NAME = test

################################################################################

# Target rules
all: build

build: $(NAME)

$(NAME): $(NAME).c
	$(COMP) $+ -o $@ $(FLAGS)

run: build
	./$(NAME)

clean:
	rm -f $(NAME)