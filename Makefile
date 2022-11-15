CC := gcc
SRCDIR := src
OBJDIR := obj
TARGET := output
CFLAGS := -Wall
LIB := -lSDL2_gfx -lSDL2_image -lSDL2_mixer -lSDL2_ttf
OBJECTS := elvono.o grafika.o liftmat.o adat.o

# Windows
ifdef OS
	RM := rmdir /S /Q
	INCLUDE += -I"$(SDL_INCLUDE)"
	LIB := -L"$(MINGW_LIB)" -lmingw32 -lSDL2main -lSDL2 $(LIB) -luser32 -lgdi32 -lwinmm -ldxguid
	MKDIR := if not exist "$(OBJDIR)" mkdir "$(OBJDIR)" & if not exist "$(TARGET)" mkdir "$(TARGET)"
# Linux
else
	RM := rm -rf
	INCLUDE += `sdl2-config --cflags`
	LIB := `sdl2-config --libs` $(LIB)
	MKDIR := mkdir -p $(OBJDIR); mkdir -p $(TARGET)
endif

default: CFLAGS += -O3
default: setupdirs Liftesch

debug: CFLAGS += -g
debug: setupdirs Liftesch

setupdirs:
	$(MKDIR)

%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $(OBJDIR)/$@ $<

Liftesch: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET)/$@ $(addprefix $(OBJDIR)/, $^) $(LIB)

clean:
	$(RM) $(BUILDDIR)