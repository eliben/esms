include makefile.linux

CCFLAGS = $(MODE) -c -Wall -pedantic -ansi

ESMS_O_FILES = \
	rosterplayer.o comment.o penalty.o report_event.o esms.o cond_utils.o \
	teamsheet_reader.o \
	cond_action.o cond_condition.o util.o cond.o mt.o config.o tactics.o anyoption.o

UPDTR_O_FILES = \
	rosterplayer.o updtr.o util.o anyoption.o config.o comment.o league_table.o

LGTABLE_O_FILES = \
	lgtable.o league_table.o util.o anyoption.o

FIXTURES_O_FILES = \
	fixtures.o util.o anyoption.o

TSC_O_FILES = \
	tsc.o rosterplayer.o util.o config.o

ROSTER_CREATOR_O_FILES = \
	roster_creator.o rosterplayer.o anyoption.o config.o util.o

.cpp.o:
	$(CC) $(CCFLAGS) $*.cpp

all: esms roster_creator lgtable updtr fixtures tsc

tsc: $(TSC_O_FILES)
	$(CC) -o tsc $(TSC_O_FILES)
	$(CP_TOOL) tsc $(CP_DEST)

roster_creator: $(ROSTER_CREATOR_O_FILES)
	$(CC) -o roster_creator $(ROSTER_CREATOR_O_FILES)
	$(CP_TOOL) roster_creator $(CP_DEST)

lgtable: $(LGTABLE_O_FILES)
	$(CC) -o lgtable $(LGTABLE_O_FILES)
	$(CP_TOOL) lgtable $(CP_DEST)

updtr: $(UPDTR_O_FILES) 
	$(CC) -o updtr $(UPDTR_O_FILES)
	$(CP_TOOL) updtr $(CP_DEST)

esms: $(ESMS_O_FILES)
	$(CC) -o esms $(ESMS_O_FILES)
	$(CP_TOOL) esms $(CP_DEST)

fixtures: $(FIXTURES_O_FILES)
	$(CC) -o fixtures $(FIXTURES_O_FILES)
	$(CP_TOOL) fixtures $(CP_DEST)

clean: 
	\rm -f $(LGTABLE_O_FILES) $(FIXTURES_O_FILES) $(ESMS_O_FILES) $(UPDTR_O_FILES) $(TSC_O_FILES) tsc esms updtr lgtable fixtures

