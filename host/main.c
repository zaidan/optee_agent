#include <stdio.h>
#include <string.h>

#include "agent_client.h"

int main(int argc, char *argv[])
{
  TEEC_UUID uuid = TA_AGENT_UUID;

  run(&uuid, run_test, argc, argv);

  return 0;
}
