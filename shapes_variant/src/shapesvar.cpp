#include <variantuint.hpp>

ACTION variantuint::addvalues() {
  name self = get_self();
  variants_table _variants(self, self.value);

  _variants.emplace(self, [&](auto& row) {
    row.id = _variants.available_primary_key();
    uint8_t var8 = 255; // largest uint8_t
    row.var = var8;
  });

  _variants.emplace(self, [&](auto& row) {
    row.id = _variants.available_primary_key();
    uint16_t var16 = 65535; // largest uint16_t
    row.var = var16;
  });
}

ACTION variantuint::printme() {
  variants_table _variants(get_self(), get_self().value);

  auto row_itr = _variants.begin();
  while (row_itr != _variants.end()) {
    print("Row id: ", row_itr->id);
    std::visit([&](auto const& var){ 
      print(" val = ", var);
    }, row_itr->var);

    ++row_itr;
  }
}

ACTION variantuint::clear() {
  require_auth(get_self());

  variants_table _variants(get_self(), get_self().value);

  auto row_itr = _variants.begin();
  while (row_itr != _variants.end()) {
    row_itr = _variants.erase(row_itr);
  }
}