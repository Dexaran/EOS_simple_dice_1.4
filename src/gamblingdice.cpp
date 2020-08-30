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
  // "12;62024e873202aff4a0466515bfd68208fd1e4353be57d65f032d184f4b24c921"
  // where the first number before `;` delimiter is the `id` of a room players wants to play at
  //       the second hex variable is `sha3_hash(<number>)` is a sha3 hash of the number that is used as entropy

  //checksum256 _hash = sha256(memo, 128);

  checksum256 _hash = sha256(memo.c_str(), sizeof(memo));

  std::string user_input = memo;

  std::string delimiter = ";";
  size_t pos = user_input.find(delimiter);
  std::string room_id;
  std::string entropy_hash;

  room_id = user_input.substr(0, pos);
  user_input.erase(0, pos + delimiter.length());

  pos = user_input.find(delimiter);
  entropy_hash = user_input.substr(0, pos);

  eosio::print("room_id", room_id);
  eosio::print("entropy_hash", entropy_hash);

  
}

EOSIO_DISPATCH(gamblingdice, (hi)(clear)(createroom))
