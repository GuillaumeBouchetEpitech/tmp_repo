
# Custom Containers

## weak_ref_data_pool

```C++
weak_ref_data_pool<
  typename InternalBaseType,
  typename PublicBaseType = InternalBaseType,
  std::size_t initial_size = 256,
  bool no_realloc = true,
  template <typename...> class Allocator = std::allocator
>;
```

### Small Example

```C++

#include "gtest/gtest.h"

#include "weak_ref_data_pool.hpp"

#include <iostream>

namespace /*anonymous*/ {

  // external / public class (interface)
  class SomePublicEntity {
  public:
    virtual ~SomePublicEntity() = default;
    virtual void update(float deltaTimeSec) = 0;
    virtual void render() const = 0;
  };

  // internal / private class (implementation)
  class SomePrivateEntity : public SomePublicEntity {
    public:
      SomePrivateEntity(std::string_view inSomeStr, int32_t inSomeInt) {
      }
      void update(float deltaTimeSec) override {
        // complex logic here
      }
      void render() const override {
        // complex logic here
      }
    private:
      // attributes here
  };

  // pool type
  using some_entities_pool = custom_containers::weak_ref_data_pool<
    SomePrivateEntity, // internal / private - type / implementation
    SomePublicEntity, // external / public - type / interface
    512, // initial size (default: 256)
    true, // no realloc (default: true)
    std::allocator // custom allocator (default: std::allocator)
  >;

  using some_weak_ref = some_entities_pool::weak_ref;
  using some_value_type = some_entities_pool::value_type; // <- SomePublicEntity
};

TEST(weak_ref_data_pool, simple_usecase) {

  some_entities_pool someEntitiesPool;

  some_weak_ref helloRef = someEntitiesPool.acquire("hello", 777);
  some_weak_ref worldRef = someEntitiesPool.acquire("world", 888);

  ASSERT_EQ(helloRef.is_valid(), true);
  ASSERT_EQ(helloRef, true); // operator bool() supported (when valid -> true)

  auto invalidRef = some_weak_ref::make_invalid(); // created invalid
  ASSERT_EQ(invalidRef.is_valid(), false);
  ASSERT_EQ(invalidRef, false);

  auto copiedRef = helloRef; // copy
  ASSERT_EQ(copiedRef.is_valid(), true);
  ASSERT_EQ(copiedRef, true);

  invalidRef = std::move(copiedRef); // move semantic supported
  ASSERT_EQ(invalidRef.is_valid(), true);
  ASSERT_EQ(invalidRef, true);

  std::swap(invalidRef, copiedRef); // ...can also use std::swap
  ASSERT_EQ(copiedRef.is_valid(), true);
  ASSERT_EQ(copiedRef, true);

  ASSERT_EQ(helloRef.is_valid(), true);
  ASSERT_EQ(copiedRef.is_valid(), true);
  ASSERT_EQ(invalidRef.is_valid(), false);

  // non-constant for-each
  someEntitiesPool.for_each([](some_value_type& inVal) -> void {
    constexpr float k_fixedStep = 1.0f / 60.0f;
    inVal.update(k_fixedStep);
  });

  // constant for-each
  someEntitiesPool.for_each([](const some_value_type& inVal) -> void {
    inVal.render();
  });

  // filtering
  someEntitiesPool.filter([](const some_value_type& inVal) -> bool {
    return true; // returning true wil keep the element, else -> fast unsorted removal
  });

  // search
  auto foundRef = someEntitiesPool.find_if([](const some_value_type& inVal) -> bool {
    return true; // returning true to get a weak_ref of the element
  });

  // all weak_ref(s) of this element are now invalid
  helloRef.invalidate_all();
  ASSERT_EQ(copiedRef.is_valid(), false);

  ASSERT_EQ(someEntitiesPool.size(), 2);
  ASSERT_EQ(someEntitiesPool.is_empty(), false);

  // fast unsorted removal of any unreferenced element(s)
  someEntitiesPool.remove_unreferenced_items();

  ASSERT_EQ(someEntitiesPool.size(), 1);
  ASSERT_EQ(someEntitiesPool.is_empty(), false);

  // manual fast unsorted removal of a referenced element
  // -> all weak_ref(s) of this element are now invalid
  someEntitiesPool.release(worldRef);

  ASSERT_EQ(someEntitiesPool.size(), 0);
  ASSERT_EQ(someEntitiesPool.is_empty(), true);

  // pool is emptied -> all existing weak_ref are invalidated
  someEntitiesPool.clear();
}

```