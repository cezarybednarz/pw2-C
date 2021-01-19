#include "cacti.h"

actor_system_t* a_system;

int actor_system_create(actor_id_t *actor, role_t *const role) {
  if (actor_system_init(a_system) != -1) {
    return -1;
  }

  a_system->role = role;
  
  // init first actor
  // todo:
  // dodać globalny bufor cykliczny (thread safe)
  // który będzie ogarniał schedulowanie pracy
  //
  // usunąć wszystkie synchronizacje, schedulowanie odbywa sie synchronicznie
  // jedyne co odbywa się rónwolegle to wywoływanie funkcji (na thread poolu)

  return 0;
}

void actor_system_join(actor_id_t actor) {

}

int send_message(actor_id_t actor, message_t message) {

}


