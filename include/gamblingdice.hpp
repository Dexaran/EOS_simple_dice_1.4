#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
#include <eosio/singleton.hpp>

using namespace std;
using namespace eosio;

CONTRACT gamblingdice : public contract {
  public:
    using contract::contract;
    
      gamblingdice( name receiver, name code, datastream<const char*> ds ) :
         // contract base class contructor
         contract(receiver, code, ds),
         // instantiate multi index instance as data member (find it defined below)
         _rooms(receiver, receiver.value),
         _messages(receiver, receiver.value),
         _debugtable(receiver, receiver.value)
         { }

    ACTION hi(name from, std::string message);
    ACTION clear();
    ACTION createroom(name player1, asset stake, uint64_t id);

    TABLE debugtable {
         name primary_value;
         uint64_t secondary_value;
         uint64_t primary_key() const { return primary_value.value; }
      } debugtable;

      using singleton_type = eosio::singleton<"debugtable"_n, debugtable>;
      singleton_type _debugtable;

    TABLE messages {
      name    user;
      string  text;
      auto primary_key() const { return user.value; }
    };
    typedef multi_index<name("messages"), messages> messages_table;
    
    TABLE rooms {
      uint64_t id;
      name    player1;
      name    player2;

      checksum256 entropy1;
      checksum256 entropy2;

      //eosio::asset   stake_test_delaration;
      //eosio::asset(0, symbol(symbol_code("SYS"),4)) stake; // Breaking change 1.2.x ->> 1.3.x
      asset   stake;

      auto primary_key() const { return id; }
    };
    typedef multi_index<name("rooms"), rooms> rooms_table;
    rooms_table _rooms;
    messages_table _messages;

  private:

    // Nothing private at the moment.
};
