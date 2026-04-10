#ifndef NEEDLE_STDIO_FILES_H
#define NEEDLE_STDIO_FILES_H

typedef struct MiniFILE MiniFILE;

MiniFILE *_Nonnull miniF_stdin_();
MiniFILE *_Nonnull miniF_stdout_();
MiniFILE *_Nonnull miniF_stderr_();

#define miniF_stdin (miniF_stdin_())
#define miniF_stdout (miniF_stdout_())
#define miniF_stderr (miniF_stderr_())

#endif //NEEDLE_STDIO_FILES_H
