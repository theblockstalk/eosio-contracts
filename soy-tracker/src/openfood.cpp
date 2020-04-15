#include <openfood.hpp>

// Assumes linear provinance chain of non-divisible and non-combineable soy
ACTION openfood::addversion(name from, uint64_t version_id, string abi) {
  require_auth(from);

  versions_table _versions(get_self(), get_self().value);

  const time_point now = current_time_point();

  auto row_itr = _versions.find(version_id);
  if (row_itr == _versions.end()) {
    _versions.emplace(from, [&](auto& row) {
      row.version_id = version_id;
      row.updated_added_by = from;
      row.updated = now;
      row.abi = abi;
    });
  } else {
    _versions.modify(row_itr, from, [&](auto& row) {
      row.updated_added_by = from;
      row.updated = now;
      row.abi = abi;
    });
  }
}

void openfood::checkVersionExists(uint64_t version_id) {
  versions_table _versions(get_self(), get_self().value);

  _versions.get(version_id, "Version id not found");
}

ACTION openfood::createsoy(name from, name to, checksum256 hash, uint64_t version_id) {
  require_auth(from);
  // require_auth(to);

  checkVersionExists(version_id);

  proofs_table _proofs(get_self(), get_self().value);

  _proofs.emplace(from, [&](auto& row) {
    row.from = from;
    row.to = to;
    row.hash = hash;
    row.version_id = version_id;
  });
}

ACTION openfood::sendsoy(name from, name to, checksum256 prev_hash, checksum256 hash, uint64_t version_id) {
  require_auth(from);
  // require_auth(to);

  checkVersionExists(version_id);
  
  proofs_table _proofs(get_self(), get_self().value);

  auto index = _proofs.get_index<"byhash"_n>();
  auto row_itr = index.find(prev_hash);
  check(row_itr != index.end(), "Previous hash is not active");
  check(row_itr->to == from, "Can only send soy that was sent to you");

  index.erase(row_itr);
  _proofs.emplace(from, [&](auto& row) {
    row.from = from;
    row.to = to;
    row.hash = hash;
    row.version_id = version_id;
  });
}