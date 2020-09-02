#include <gamblingdice.hpp>

//using namespace gamblingdice;

template<typename CharT>
static std::string to_hex(const CharT* d, uint32_t s) {
  std::string r;
  const char* to_hex="0123456789abcdef";
  uint8_t* c = (uint8_t*)d;
  for( uint32_t i = 0; i < s; ++i ) {
    (r += to_hex[(c[i] >> 4)]) += to_hex[(c[i] & 0x0f)];
  }
  return r;
}

static std::string to_sha256_string(std::string str) {
  checksum256 digest;
  digest = sha256(&str[0], str.size());

  return to_hex(&digest, sizeof(digest));
}

ACTION gamblingdice::hi(const eosio::name from, eosio::name to, eosio::asset quantity, std::string message) {
  //require_auth(from);

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

[[eosio::on_notify("eosio.token::transfer")]] void gamblingdice::deposit(const name from, name to, asset quantity, std::string memo) {
  // Memo format:
  // "12;62024e873202aff4a0466515bfd68208fd1e4353be57d65f032d184f4b24c921"
  // where the first number before `;` delimiter is the `id` of a room players wants to play at
  //       the second hex variable is `sha3_hash(<number>)` is a sha3 hash of the number that is used as entropy

  //checksum256 _hash = sha256(memo, 128);

  std::string _test_str = "100";

  checksum256 _hash = sha256(&_test_str[0], _test_str.size());
  std::string _hashString = to_sha256_string("100");

/*
  _messages.emplace(get_self(), [&](auto& msg) {
      msg.user = from;
      msg.text = memo;
    });
*/

  if (!singleton_debug.exists())
   {
      singleton_debug.get_or_create(get_self(), tt); // get_or_create( <payer> , <row_template_struct> );
   }

  std::string user_input = memo;

  std::string delimiter = ";";
  size_t pos = user_input.find(delimiter);
  std::string room_id;
  std::string entropy_hash;

  room_id = user_input.substr(0, pos);
  user_input.erase(0, pos + delimiter.length());

  pos = user_input.find(delimiter);
  entropy_hash = user_input.substr(0, pos);

  // ######################################
  // ASSIGN DEBUG VARIABLES
  //

  auto entry_stored            = singleton_debug.get();
  entry_stored.debug_string1   = _hashString;
  entry_stored.debug_checksum1 = sha256(&room_id[0], room_id.size());
  entry_stored.debug_uint1     = std::strtoull (room_id.c_str(), NULL, 10);

/*
  if(_hash.to_string() == entropy_hash) {
    entry_stored.debug_uint1 = 1;
  } else {
    entry_stored.debug_uint1 = 2;
  }
  */

  singleton_debug.set(entry_stored, get_self());
}

//EOSIO_DISPATCH(gamblingdice, (hi)(clear)(createroom))
