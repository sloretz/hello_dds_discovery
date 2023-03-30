#include "dds/dds.h"
#include "HelloWorld.h"
#include <stdio.h>
#include <stdlib.h>

const char * kConfig =
"<CycloneDDS>"
" <Domain>"
"   <General>"
"     <AllowMulticast>false</AllowMulticast>"
"   </General>"
"   <Discovery>"
"     <ParticipantIndex>auto</ParticipantIndex>"
"     <Peers>"
"       <Peer Address=\"127.0.0.1\"/>"
"     </Peers>"
"   </Discovery>"
" </Domain>"
"</CycloneDDS>";

int main (int argc, char ** argv)
{
  dds_entity_t domain_handle;
  dds_entity_t participant;
  dds_entity_t topic;
  dds_entity_t writer;
  dds_return_t rc;
  HelloWorld msg;
  uint32_t status = 0;
  (void)argc;
  (void)argv;

  /* Configure a domain with custom XML */
  domain_handle= dds_create_domain(0, kConfig);
  if (domain_handle < 0 ) {
    DDS_FATAL("dds_create_domain: %s\n", dds_strretcode(-domain_handle));
  }

  /* Create a Participant. */
  participant = dds_create_participant (0, NULL, NULL);
  if (participant < 0)
    DDS_FATAL("dds_create_participant: %s\n", dds_strretcode(-participant));

  /* Create a Topic. */
  topic = dds_create_topic (
    participant, &HelloWorld_desc, "HelloWorld", NULL, NULL);
  if (topic < 0)
    DDS_FATAL("dds_create_topic: %s\n", dds_strretcode(-topic));

  /* Create a Writer. */
  writer = dds_create_writer (participant, topic, NULL, NULL);
  if (writer < 0)
    DDS_FATAL("dds_create_writer: %s\n", dds_strretcode(-writer));

  printf("=== [Publisher]  Waiting for a reader to be discovered ...\n");
  fflush (stdout);

  rc = dds_set_status_mask(writer, DDS_PUBLICATION_MATCHED_STATUS);
  if (rc != DDS_RETCODE_OK)
    DDS_FATAL("dds_set_status_mask: %s\n", dds_strretcode(-rc));

  while(!(status & DDS_PUBLICATION_MATCHED_STATUS))
  {
    rc = dds_get_status_changes (writer, &status);
    if (rc != DDS_RETCODE_OK)
      DDS_FATAL("dds_get_status_changes: %s\n", dds_strretcode(-rc));

    /* Polling sleep. */
    dds_sleepfor (DDS_MSECS (20));
  }

  /* Create a message to write. */
  msg.message = "Hello World";

  while (1) {
    printf ("=== [Publisher]  Writing : ");
    printf ("Message: %s\n", msg.message);
    fflush (stdout);

    rc = dds_write (writer, &msg);
    if (rc != DDS_RETCODE_OK)
        DDS_FATAL("dds_write: %s\n", dds_strretcode(-rc));

    dds_sleepfor (DDS_MSECS (1000));
  }

  /* Deleting the participant will delete all its children recursively as well. */
  rc = dds_delete (participant);
  if (rc != DDS_RETCODE_OK)
    DDS_FATAL("dds_delete: %s\n", dds_strretcode(-rc));

  rc = dds_delete(domain_handle);
  if (rc != DDS_RETCODE_OK)
    DDS_FATAL("dds_delete: %s\n", dds_strretcode(-rc));

  return EXIT_SUCCESS;
}
