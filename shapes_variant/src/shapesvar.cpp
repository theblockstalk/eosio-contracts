#include <variantuint.hpp>

ACTION variantuint::createme() {
  name self = get_self();
  variants_table _variants(self, self.value);

  _variants.emplace(self, [&](auto& row) {
    row.id = _variants.available_primary_key();
    uint8_t uin8 = 255; // largest uint8_t
    row.var = uin8;
  });

  _variants.emplace(self, [&](auto& row) {
    row.id = _variants.available_primary_key();
    uint16_t uin16 = 65535; // largest uint16_t
    row.var = uin16;
  });

  _variants.emplace(self, [&](auto& row) {
    row.id = _variants.available_primary_key();
    string str = "happy days";
    row.var = str;
  });
}

ACTION variantuint::updateme() {
  name self = get_self();
  variants_table _variants(self, self.value);

  auto row_itr = _variants.begin();
  while (row_itr != _variants.end())
  {
    std::variant<uint8_t, uint16_t, string> var = row_itr->var;
    std::variant<uint8_t, uint16_t, string> newvar;
    switch(var.index())
    {
      case 0:
        {
          uint8_t newuint8 = std::get<uint8_t>(var) - 1;
          newvar = newuint8;
          break;
        }
      case 1:
        {
          uint16_t newuint16 = std::get<uint16_t>(var) - 1;
          newvar = newuint16;
          break;
        }
      case 2:
      {
        string oldstr = std::get<string>(var);
        string newstr = oldstr + oldstr;
        newvar = newstr;
        break;
      }
    }
   _variants.modify(row_itr, self, [&](auto& row) {
      row.var = newvar;
    });
    ++row_itr;
  }
}

ACTION variantuint::printme() {
  variants_table _variants(get_self(), get_self().value);

  auto row_itr = _variants.begin();
  while (row_itr != _variants.end()) {
    print("Row(", row_itr->id);
    std::visit([&](auto const& var){ 
      print(", ", var, "), ");
    }, row_itr->var);

    ++row_itr;
  }
}

ACTION variantuint::clear() {
  name self = get_self();
  require_auth(self);

  variants_table _variants(self, self.value);

  auto row_itr = _variants.begin();
  while (row_itr != _variants.end()) {
    row_itr = _variants.erase(row_itr);
  }
}