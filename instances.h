#define INSTANCESMAXINBOXES			5
#define INSTANCESMAXINSTANCES		10
#define INSTANCESCOMMANDMAXCHARS	256
#define INSTANCESLABELMAXCHARS		256
#define INSTANCESCACHEMAXCHARS		4096
#define INSTANCESCACHEMAXINTS		1024
#define INSTANCESCACHEMAXFLOATS		1024


typedef enum {
  CACHETYPENOTHING, CACHETYPEINVALID, CACHETYPESTRING,
  CACHETYPECOLOURSTRING, CACHETYPEJSONOBJECT,
  CACHETYPEINTARRAY, CACHETYPEFLOATARRAY
} cachetype_e;

typedef union {
  char arrayc[INSTANCESCACHEMAXCHARS WITHNULL];
  int arrayi[INSTANCESCACHEMAXINTS];
  float arrayf[INSTANCESCACHEMAXFLOATS];
} cachedata_u;

typedef struct instance_st {
  int inboxindex;
  char command[INSTANCESCOMMANDMAXCHARS WITHNULL];
  char *commandnamenull;
  char plainheader[INSTANCESLABELMAXCHARS WITHNULL];
  char colourheader[INSTANCESLABELMAXCHARS WITHNULL];
  char plainfooter[INSTANCESLABELMAXCHARS WITHNULL];
  char colourfooter[INSTANCESLABELMAXCHARS WITHNULL];
  cachetype_e cachetype;
  cachedata_u cachedata;
  int cachesize;
  int backgroundpid;
  struct instance_st *previnstance;
  struct instance_st *nextinstance;
} instance_s;

typedef struct {
  instance_s instances[INSTANCESMAXINSTANCES];
  int instancesn;
  instance_s *forinbox[INSTANCESMAXINBOXES];
} instancesof_s;
instancesof_s instances;

void instancesinit() {
  instances.instancesn = 0;
  int i = 0;
  while (i < INSTANCESMAXINBOXES)
    { instances.forinbox[i] = NULL; i++; }
}

// Adds new cache instance associated with inboxindex, using new memory where
// possible and LILO buffering where not. Returns a pointer to the instance.
instance_s *instancesaddcache(int inboxindex, char *command) {
  if (inboxindex < 0 || inboxindex >= INSTANCESMAXINBOXES) { return NULL; }
  instance_s *ins = NULL;
  if (instances.instancesn >= INSTANCESMAXINSTANCES) { return NULL; }
  else if (instances.instancesn == INSTANCESMAXINSTANCES - 1) {
    // find oldest instance available in stream for this inbox
    ins = instances.forinbox[inboxindex];
    while (ins->previnstance != NULL) {
      ins = ins->previnstance;
      if (ins->previnstance == NULL) {
        // oldest (ins) becomes newest, second-oldest becomes oldest
        ins->nextinstance->previnstance = NULL;
        ins->previnstance = instances.forinbox[inboxindex];
        ins->previnstance->nextinstance = ins;
        instances.forinbox[inboxindex] = ins;
        ins->nextinstance = NULL;
        break;
      }
    }
  } else {
    // use next free instance index
    ins = &(instances.instances[instances.instancesn]);
    instances.instancesn++;
    if (instances.forinbox[inboxindex] == NULL) {
      // this is the first for this inbox
      ins->previnstance = NULL;
      instances.forinbox[inboxindex] = ins;
      ins->nextinstance = NULL;
    } else {
      // this is the next for this inbox
      ins->previnstance = instances.forinbox[inboxindex];
      ins->previnstance->nextinstance = ins;
      instances.forinbox[inboxindex] = ins;
      ins->nextinstance = NULL;
    }
  }
  // save the deets
  ins->inboxindex = inboxindex;
//  ins->boxindex = inboxnindex[inboxindex]; <-- backwards
  strtostr(command, '\0', ins->command, '\0');
  ins->commandnamenull = &(ins->command[0]);
  ins->plainheader[0] = '\0';
  ins->colourheader[0] = '\0';
  ins->plainfooter[0] = '\0';
  ins->colourfooter[0] = '\0';
  ins->cachetype = CACHETYPENOTHING;
  ins->cachesize = 0;
  char ocn = *(ins->commandnamenull);
  while (ocn != ' ' && ocn != '\0')
    { ins->commandnamenull++; ocn = *(ins->commandnamenull); }
  ins->backgroundpid = 0;
  return ins;
}

void instancesfillcache(int inboxindex, cachetype_e type, void *data) {
  if (inboxindex < 0 || inboxindex >= INSTANCESMAXINBOXES) { return; }
  instance_s *ins = instances.forinbox[inboxindex];
  if (type == CACHETYPESTRING) {
    ins->cachetype = type;
    ins->cachesize = strtostr((char *)data, '\0', ins->cachedata.arrayc, '\0');
  } else
    { ins->cachetype = CACHETYPEINVALID; ins->cachesize = 0; }
}

void instancesfillcacheaddchar(int inboxindex, char ch) {
  if (inboxindex < 0 || inboxindex >= INSTANCESMAXINBOXES) { return; }
  instance_s *ins = instances.forinbox[inboxindex];
  if (ins->cachetype == CACHETYPESTRING && ins->cachesize < INSTANCESCACHEMAXCHARS) {
    ins->cachedata.arrayc[ins->cachesize] = (ch < ' ' || ch > '~') ? '~' : ch;
    ins->cachesize++;
    ins->cachedata.arrayc[ins->cachesize] = '\0';
  } else
    { ins->cachetype = CACHETYPEINVALID; ins->cachesize = 0; }
}

char *instancesstringcacheforinbox(int inboxindex) {
  if (inboxindex < 0 || inboxindex >= INSTANCESMAXINBOXES) { return NULL; }
  instance_s *ins = instances.forinbox[inboxindex];
  return (ins == NULL) ? NULL : ins->cachedata.arrayc;
}

int instancessetbackgroundpidforinbox(int inboxindex, int backgroundpid) {
  if (inboxindex < 0 || inboxindex >= INSTANCESMAXINBOXES) { return 0; }
  instance_s *ins = instances.forinbox[inboxindex];
  if (ins == NULL) { return 0; }
  else { ins->backgroundpid = backgroundpid; return 1; }
}
