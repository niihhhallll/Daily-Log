# Day - 1 [SUNDAY]

### [1] Schedule
- **Start time:** 7:00 AM
- **Topics today:** References vs Pointers → RAII & Smart Pointers → Linked Lists

---

## [2] References vs Pointers

### The Core Idea
A **pointer** is a variable that stores a memory address. It has its own storage, can be reseated (pointed elsewhere), can be `nullptr`, and needs explicit dereferencing (`*ptr`) to access the value it points to.

A **reference** is an alias for an existing variable. Once bound, it cannot be reseated, cannot be null (in well-formed code), and behaves syntactically just like the original variable — no dereferencing needed.

### Code Example
```cpp
#include <iostream>

int main() {
    int a = 10;
    int b = 20;

    // ---- Pointer example ----
    // - Pointers cost extra memory (they store an address, usually 8 bytes on 64-bit systems)
    // - They can be nullptr
    // - They can be reassigned to point at a different variable
    int* ptr = &a;
    std::cout << "ptr points to a, value = " << *ptr << "\n";
    ptr = &b; // reseated -> now points to b
    std::cout << "ptr now points to b, value = " << *ptr << "\n";

    // ---- Reference example ----
    // - This is an alias, not a separate object
    // - Mostly used to pass arguments into functions without copying
    // - Cannot be reseated once bound
    int& ref = a;
    ref = 50; // this actually changes 'a'
    std::cout << "a is now " << a << " (changed through ref)\n";

    return 0;
}
```

### Explanation
- `ptr` holds the **address** of `a`, then later the address of `b`. Dereferencing with `*ptr` gives you the value at that address.
- `ref` is bound to `a` at declaration and forever refers to `a`. Assigning to `ref` is the same as assigning to `a` — there's no "reseating" syntax for references.
- Pointers introduce a layer of indirection that references hide from you syntactically, even though under the hood references are usually implemented as pointers by the compiler.

### Practice Programs — References & Pointers
1. **Swap two integers** using pointers, then rewrite it using references. Compare readability.
2. **Null-check practice:** write a function that takes an `int*`, checks for `nullptr`, and prints the value if valid. Then try to do the same with a reference — notice why you *can't* meaningfully null-check a reference.
3. **Pointer arithmetic:** declare an `int arr[5]`, get a pointer to the first element, and walk through the array using `ptr++`.
4. **Reference in a loop:** use `for (int& x : vec)` to double every element of a `std::vector<int>` in place, then try it with `for (int x : vec)` and observe that the original vector is unchanged.
5. **Function overload test:** write two functions, `void f(int)` and `void f(int&)`, and see which one binds when you call `f(a)` vs `f(5)`.

### When to Use Which

**1. Passing a variable to a function**

```cpp
// Don't
// C-style, outdated, harder to read, and requires manual null checks
int Func(int* a) {
    if (a == nullptr) {
        return 1;
    }
    std::cout << *a << std::endl;
    return 0;
}

// Do
// Cleaner syntax, easier to read, safer — no null dereference risk
int Func(int& a) {
    std::cout << a << std::endl;
    return 0;
}
```

**2. If the argument should be optional**
Use a pointer, since references can't represent "no value" cleanly:
```cpp
int FuncOptional(int* a = nullptr) {
    if (a) {
        std::cout << "Got: " << *a << std::endl;
    } else {
        std::cout << "No value provided" << std::endl;
    }
    return 0;
}
```
> Modern alternative: `std::optional<int>` or `std::optional<std::reference_wrapper<int>>` is often a cleaner, type-safe replacement for "optional pointer" in modern C++ (C++17+).

### Quick Decision Table

| Situation | Use |
|---|---|
| Must accept "no value" (optional argument) | Pointer (or `std::optional`) |
| Need to reseat / point to different objects over time | Pointer |
| Passing a variable into a function, no reassignment needed | Reference |
| Want compiler-enforced non-null guarantee | Reference |
| Working with dynamic memory / arrays / low-level buffers | Pointer |
| Operator overloading, range-based for loops | Reference |

### Is This Optimizable? How to Go Faster
- References avoid an extra indirection in *some* cases because the compiler can often keep the referenced object "in mind" without re-loading an address — but in practice, with optimizations off, references and pointers compile to nearly identical machine code.
- **Pass by `const&` for large objects** (structs, `std::string`, `std::vector`, etc.) instead of by value — this avoids expensive copies.
- **Pass by value for small trivial types** (`int`, `double`, `bool`) — passing by reference here can actually be *slower* because it forces a memory load instead of keeping the value in a register.
- Compile with `-O2` or `-O3` (GCC/Clang) — the optimizer will frequently eliminate the pointer/reference distinction entirely once it can prove the object doesn't escape or alias.
- Use the `restrict`/`__restrict` keyword (or the C++20 equivalent reasoning) when you can guarantee two pointers never alias the same memory — this unlocks vectorization.
- Prefer references over pointers when possible: fewer null checks means fewer branches, and fewer branches means better CPU branch prediction.

---

## [3] RAII + Smart Pointers

### What is RAII?
**RAII (Resource Acquisition Is Initialization)** is a C++ idiom where a resource (memory, file handle, mutex lock, socket, etc.) is acquired in a constructor and released in the destructor. Because C++ guarantees destructors run when an object goes out of scope — even during stack unwinding from an exception — RAII gives you automatic, deterministic cleanup with zero manual bookkeeping.

### Smart Pointers (RAII applied to memory)
Smart pointers are RAII wrappers around raw pointers. The three main ones from `<memory>`:

- **`std::unique_ptr<T>`** — sole ownership. Cannot be copied, only moved. Zero overhead compared to a raw pointer (no reference counting).
- **`std::shared_ptr<T>`** — shared ownership via reference counting. The object is destroyed when the last `shared_ptr` owning it is destroyed.
- **`std::weak_ptr<T>`** — a non-owning observer of a `shared_ptr`-managed object. Used to break reference cycles.

### Code: RAII Class + Smart Pointers
```cpp
#include <iostream>
#include <memory>

// ---- A hand-written RAII wrapper ----
class FileHandle {
public:
    FileHandle(const std::string& name) {
        std::cout << "Opening file: " << name << "\n";
        // pretend we opened a file handle here
    }
    ~FileHandle() {
        std::cout << "Closing file automatically (RAII)\n";
        // pretend we closed the file handle here
    }
};

void RaiiDemo() {
    FileHandle f("data.txt");
    // even if an exception is thrown here, ~FileHandle() still runs
    std::cout << "Using file...\n";
} // f goes out of scope -> destructor runs automatically

// ---- unique_ptr: sole ownership ----
void UniquePtrDemo() {
    std::unique_ptr<int> up = std::make_unique<int>(42);
    std::cout << "unique_ptr value: " << *up << "\n";
    // std::unique_ptr<int> up2 = up; // ERROR: can't copy
    std::unique_ptr<int> up2 = std::move(up); // OK: ownership transferred
}

// ---- shared_ptr: shared ownership with reference counting ----
void SharedPtrDemo() {
    std::shared_ptr<int> sp1 = std::make_shared<int>(100);
    std::shared_ptr<int> sp2 = sp1; // both now own the same int
    std::cout << "use_count: " << sp1.use_count() << "\n"; // 2
} // both sp1 and sp2 go out of scope -> memory freed here

// ---- weak_ptr: breaking cycles ----
struct Node {
    std::shared_ptr<Node> next;
    std::weak_ptr<Node> prev; // weak_ptr avoids a reference-count cycle
};
```

### Explanation
- `RaiiDemo` never calls a "close" function manually — the destructor does it, guaranteed, even on exceptions.
- `UniquePtrDemo` shows that ownership must be explicitly moved; this prevents accidental double-ownership bugs at compile time.
- `SharedPtrDemo` shows two smart pointers sharing one heap allocation; the underlying `int` is freed only when the last owner disappears.
- The `Node` struct shows the classic cycle problem: if `next` and `prev` were both `shared_ptr`, two nodes pointing at each other would never reach a reference count of zero, causing a memory leak. Using `weak_ptr` for the back-pointer breaks the cycle.

### Pros and Cons

| | Pros | Cons |
|---|---|---|
| **RAII (general)** | Deterministic cleanup, exception-safe, no manual free/close calls, less boilerplate | Requires discipline to wrap *every* resource; can obscure control flow if overused |
| **`unique_ptr`** | Zero runtime overhead vs raw pointer, clear single-ownership semantics | Can't be copied — must restructure code around moves; adds a tiny bit of API friction |
| **`shared_ptr`** | Automatic memory management with shared ownership, thread-safe ref counting | Atomic increment/decrement on every copy = performance cost; risk of reference cycles/leaks; larger memory footprint (control block) |
| **`weak_ptr`** | Solves the cycle problem, safe way to "peek" without extending lifetime | Extra `.lock()` call needed to use it; slightly more complex API |

### Is This Optimizable?
- **Prefer `unique_ptr` by default.** Only reach for `shared_ptr` when ownership is genuinely shared across multiple owners with unclear lifetime.
- **Use `make_unique`/`make_shared` instead of `new`.** `make_shared` in particular does a single heap allocation for both the object and its control block, instead of two separate allocations.
- **Avoid `shared_ptr` in hot loops.** Its atomic ref-counting has real, measurable overhead under contention. If ownership never actually needs to be shared, this cost is pure waste.
- **Pass smart pointers by `const&` when you don't need to change ownership**, e.g. `void f(const std::unique_ptr<int>& p)` or better, just pass `int*`/`int&` if the function doesn't need to participate in ownership at all — "don't pass a smart pointer just to look at the value."
- **Use `.get()` sparingly** to interoperate with raw-pointer APIs, but never store the raw pointer beyond the smart pointer's lifetime.
- Compile with `-O2`/`-O3`; the compiler can often inline `unique_ptr` operations down to the same code as raw pointers.

### Practice Programs — RAII & Smart Pointers
1. Write a `ScopedTimer` RAII class that prints elapsed time from constructor to destructor.
2. Build a `unique_ptr`-based binary tree where each node owns its children.
3. Build a small doubly linked structure using `shared_ptr` for `next` and `weak_ptr` for `prev`; print `use_count()` at each step to see the ref count change.
4. Write a function that returns a `unique_ptr<T>` by value and observe move semantics (no copy happens).
5. Deliberately create a `shared_ptr` reference cycle (two nodes pointing at each other) and confirm with a printed destructor message that memory is *not* freed — then fix it with `weak_ptr` and confirm it now frees correctly.

---

## [4] Linked Lists — Full Crash Course

### Why Linked Lists Are Useful
A **linked list** is a linear data structure where each element ("node") holds data plus a pointer to the next node. Unlike arrays, nodes don't need to sit in contiguous memory.

**Useful because:**
- O(1) insertion/removal at the front (and at any known position, given a pointer to it) — no shifting of elements like an array requires.
- Dynamic size — grows and shrinks without reallocating a whole contiguous block.
- No wasted pre-allocated capacity, unlike `std::vector`'s growth strategy.

### Pros and Cons

| Pros | Cons |
|---|---|
| O(1) insert/delete at head (or anywhere with an iterator/pointer) | O(n) random access — no `list[i]` in constant time |
| No need for contiguous memory | Poor cache locality — nodes are scattered in memory, hurting performance |
| Easy to grow/shrink dynamically | Extra memory per node for storing the pointer(s) |
| No costly reallocation/copy when growing (unlike a full vector resize) | More pointer chasing = more cache misses = often slower in practice than arrays for sequential access |

### Is It Memory Efficient?
Not particularly, compared to arrays:
- Every node pays overhead for the pointer field(s) — on a 64-bit system, a singly linked list node with an `int` payload pays 4 bytes of data + 8 bytes for the pointer (plus padding), which is a large overhead ratio for small payloads.
- Nodes are heap-allocated individually, which is slower than one contiguous allocation and fragments memory.
- Cache locality is poor: because nodes can live anywhere in memory, walking the list causes frequent cache misses, whereas an array's contiguous layout lets the CPU prefetch efficiently.
- **In short:** linked lists win on insertion/deletion flexibility, but lose on raw memory efficiency and cache-friendly traversal compared to `std::vector`.

### Code: Singly Linked List (Insert, Delete, Search, Display)
```cpp
#include <iostream>

struct Node {
    int data;
    Node* next;
    Node(int val) : data(val), next(nullptr) {}
};

class LinkedList {
private:
    Node* head;

public:
    LinkedList() : head(nullptr) {}

    ~LinkedList() {
        // RAII: clean up every node when the list is destroyed
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }

    // Insert at the front - O(1)
    void insertFront(int value) {
        Node* newNode = new Node(value);
        newNode->next = head;
        head = newNode;
    }

    // Insert at the end - O(n), since we must walk to the last node
    void insertEnd(int value) {
        Node* newNode = new Node(value);
        if (head == nullptr) {
            head = newNode;
            return;
        }
        Node* current = head;
        while (current->next != nullptr) {
            current = current->next;
        }
        current->next = newNode;
    }

    // Delete the first node with a given value - O(n)
    void deleteValue(int value) {
        if (head == nullptr) return;

        if (head->data == value) {
            Node* toDelete = head;
            head = head->next;
            delete toDelete;
            return;
        }

        Node* current = head;
        while (current->next != nullptr && current->next->data != value) {
            current = current->next;
        }

        if (current->next != nullptr) {
            Node* toDelete = current->next;
            current->next = toDelete->next;
            delete toDelete;
        }
    }

    // Search for a value - O(n)
    bool search(int value) const {
        Node* current = head;
        while (current != nullptr) {
            if (current->data == value) return true;
            current = current->next;
        }
        return false;
    }

    // Display the whole list - O(n)
    void display() const {
        Node* current = head;
        while (current != nullptr) {
            std::cout << current->data;
            if (current->next != nullptr) std::cout << " -> ";
            current = current->next;
        }
        std::cout << "\n";
    }
};

int main() {
    LinkedList list;
    list.insertFront(10);
    list.insertFront(20);
    list.insertEnd(30);
    list.display();          // 20 -> 10 -> 30

    list.deleteValue(10);
    list.display();          // 20 -> 30

    std::cout << "Contains 30? " << list.search(30) << "\n";  // 1 (true)
    std::cout << "Contains 99? " << list.search(99) << "\n";  // 0 (false)

    return 0;
}
```

### How Each Method Works
- **`insertFront`**: creates a new node, points it at the current head, then makes it the new head. Constant time because no traversal is needed.
- **`insertEnd`**: must walk the entire list to find the last node (the one whose `next` is `nullptr`), then attach the new node there. Linear time.
- **`deleteValue`**: special-cases deleting the head (just move `head` forward), otherwise walks the list keeping a "current" pointer one step behind the node to delete, so it can relink `current->next` around the deleted node.
- **`search`**: simple linear walk comparing each node's data to the target value.
- **`display`**: linear walk printing each node's value, with arrows between them for readability.
- **Destructor**: this is RAII in action again — the list itself owns all its nodes, so when the `LinkedList` object is destroyed, its destructor frees every node, preventing memory leaks.

### Is This Optimizable? How to Run It Faster
- **Track a `tail` pointer** so `insertEnd` becomes O(1) instead of O(n) — avoids walking the whole list every time you append.
- **Track a `size` counter** so you don't need an O(n) walk just to know how many elements exist.
- **Use a memory pool / object pool allocator** for nodes instead of calling `new`/`delete` per node — this drastically reduces heap allocation overhead and improves cache locality since pooled nodes tend to live closer together in memory.
- **Consider `std::forward_list`** (singly linked) or `std::list` (doubly linked) from the STL instead of hand-rolling one — they're well-tested and often have allocator customization built in.
- **Prefer a `std::vector` unless you specifically need O(1) mid-list insertion/removal.** For most workloads dominated by iteration or random access, a `vector`'s cache-friendly contiguous memory beats a linked list's pointer-chasing, even though vector insertion in the middle is technically O(n).
- **Replace raw `new`/`delete` with smart pointers (`unique_ptr<Node>`)** for automatic cleanup and exception safety — trades a small amount of performance for a big safety win; worth it unless profiling shows it's a bottleneck.
- **Batch allocate nodes** in chunks (e.g., allocate arrays of 64 nodes at a time from a pool) rather than one-by-one, to reduce allocator calls and improve locality.
- Compile with `-O2`/`-O3` and consider `-march=native` if targeting a specific CPU, to let the compiler make the best use of the available instruction set.

### Practice Programs — Linked Lists
1. Add a `tail` pointer to the `LinkedList` class above and make `insertEnd` O(1). Benchmark before/after on a list of 100,000 elements.
2. Implement `reverse()` — reverse the entire linked list in place, O(n) time, O(1) extra space.
3. Implement cycle detection using Floyd's Tortoise and Hare algorithm.
4. Convert the singly linked list into a **doubly linked list** (add a `prev` pointer) and implement `insertBefore(Node*, value)`.
5. Rewrite the list using `std::unique_ptr<Node>` for ownership instead of raw `new`/`delete`, and confirm (via print statements in the destructor) that all nodes are cleaned up automatically.
6. Write a benchmark comparing insert-at-front performance and full-traversal performance between your `LinkedList` and `std::vector<int>` for 1,000,000 elements — observe how cache locality affects the numbers.

---

## Summary Checklist for Today
- [ ] References vs Pointers — read theory, run all 5 practice programs
- [ ] RAII & Smart Pointers — read theory, run all 5 practice programs
- [ ] Linked Lists — read theory, run all 6 practice programs
- [ ] Benchmark at least one "before optimization vs after optimization" comparison using `-O2`/`-O3`
