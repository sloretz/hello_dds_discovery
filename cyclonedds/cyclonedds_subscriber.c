#include "dds/dds.h"
#include "HelloWorld.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* An array of one message (aka sample in dds terms) will be used. */
#define MAX_SAMPLES 1

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
  dds_entity_t reader;
  HelloWorld *msg;
  void *samples[MAX_SAMPLES];
  dds_sample_info_t infos[MAX_SAMPLES];
  dds_return_t rc;
  dds_qos_t *qos;
  (void)argc;
  (void)argv;

  /* Configure a domain with custom XML */
  domain_handle= dds_create_domain(0, kConfig);
  if (domain_handle < 0 ) {
    DDS_FATAL("dds_create_domain: %s\n", dds_strretcode(-domain_handle));
  }

  /* Create a Participant. */
  participant = dds_create_participant (DDS_DOMAIN_DEFAULT, NULL, NULL);
  if (participant < 0)
    DDS_FATAL("dds_create_participant: %s\n", dds_strretcode(-participant));

  /* Create a Topic. */
  topic = dds_create_topic (
    participant, &HelloWorld_desc, "HelloWorld", NULL, NULL);
  if (topic < 0)
    DDS_FATAL("dds_create_topic: %s\n", dds_strretcode(-topic));

  /* Create a reliable Reader. */
  qos = dds_create_qos ();
  dds_qset_reliability (qos, DDS_RELIABILITY_RELIABLE, DDS_SECS (10));
  reader = dds_create_reader (participant, topic, qos, NULL);
  if (reader < 0)
    DDS_FATAL("dds_create_reader: %s\n", dds_strretcode(-reader));
  dds_delete_qos(qos);

  printf ("\n=== [Subscriber] Waiting for a sample ...\n");
  fflush (stdout);

  /* Initialize sample buffer, by pointing the void pointer within
   * the buffer array to a valid sample memory location. */
  samples[0] = HelloWorld__alloc ();

  /* Poll until data has been read. */
  while (true)
  {
    /* Do the actual read.
     * The return value contains the number of read samples. */
    rc = dds_take(reader, samples, infos, MAX_SAMPLES, MAX_SAMPLES);
    if (rc < 0)
      DDS_FATAL("dds_read: %s\n", dds_strretcode(-rc));

    /* Check if we read some data and it is valid. */
    if ((rc > 0) && (infos[0].valid_data))
    {
      /* Print Message. */
      msg = (HelloWorld*) samples[0];
      printf ("=== [Subscriber] Received : ");
      printf ("Message: %s\n", msg->message);
      fflush (stdout);
    }
    else
    {
      /* Polling sleep. */
      dds_sleepfor (DDS_MSECS (20));
    }

  }

  /* Free the data location. */
  HelloWorld_free (samples[0], DDS_FREE_ALL);

  /* Deleting the participant will delete all its children recursively as well. */
  rc = dds_delete (participant);
  if (rc != DDS_RETCODE_OK)
    DDS_FATAL("dds_delete: %s\n", dds_strretcode(-rc));

  rc = dds_delete(domain_handle);
  if (rc != DDS_RETCODE_OK)
    DDS_FATAL("dds_delete: %s\n", dds_strretcode(-rc));

  return EXIT_SUCCESS;
}
