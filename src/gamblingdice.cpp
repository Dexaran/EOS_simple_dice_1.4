#include <gamblingdice.hpp>

ACTION gamblingdice::hi(name from, string message) {
  require_auth(from);

  // Init the _message table
  messages_table _messages(get_self(), get_self().value);

  // Find the record from _messages table
  auto msg_itr = _messages.find(from.value);
  if (msg_itr == _messages.end()) {
    // Create a message record if it does not exist
    _messages.emplace(from, [&](auto& msg) {
      msg.user = from;
      msg.text = message;
    });
  } else {
    // Modify a message record if it exists
    _messages.modify(msg_itr, from, [&](auto& msg) {
      msg.text = message;
    });
  }
}

ACTION gamblingdice::clear() {
  require_auth(get_self());

  messages_table _messages(get_self(), get_self().value);

  // Delete all records in _messages table
  auto msg_itr = _messages.begin();
  while (msg_itr != _messages.end()) {
    msg_itr = _messages.erase(msg_itr);
  }
}

ACTION gamblingdice::createroom(name player1, asset stake, uint64_t id) {
  require_auth(player1);

  auto itr = _rooms.find(id);
  check(itr == _rooms.end(), "The room already exists!");

  _rooms.emplace(get_self(), [&](auto &new_room) {
      new_room.player1 = player1;
      new_room.stake   = stake;
      new_room.id      = id;
    });
}

[[eosio::on_notify("eosio.token::transfer")]]
void deposit(name from, name to, asset quantity, std::string memo) {
  // Memo format:
  // "room-12 entropy-62024e873202aff4a0466515bfd68208fd1e4353be57d65f032d184f4b24c921"
  // where `room`-`<number>` is the `id` of a room players wants to play at
  //       `entropy`-`sha3_hash(<number>)` is a sha3 hash of the number that is used as entropy

  //checksum256 _hash = sha256(memo, 128);

}

EOSIO_DISPATCH(gamblingdice, (hi)(clear)(createroom))
