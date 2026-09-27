//#include <algorithm>
//#include <bit_factory/v26/anys/range/iterators.hpp>
//#include <bit_factory/v26/trait_as.hpp>
//#include <catch2/catch_test_macros.hpp>
//#include <iostream>
//#include <list>
//#include <vector>
//#include <print>
//
//// iterator adapter like boost::iterator_adaptor based on
//// anyxx forward_iterator
//namespace {
//using namespace anyxx26;
//template <is_trait Trait, typename Self, typename Value>
//struct dummy { //: //save_copyable<Trait, Self> {
//    using default_proxy_t = anyxx::val<std::true_type>;
//
//    Self id(Self const& self) { return self; }
//    //bool operator==(Self const&) const;
//    //bool operator!=(Self const&) const;
//    //Self& operator++();
//
//    ////Ref operator*() const;
//    //Value& op_star(Self const& self) {
//    //    if constexpr(std::is_reference_v<std::remove_const_t<Value>>) {
//    //        return const_cast<Value>(*self);
//    //    } else {
//    //        return *self;
//    //    }
//    //}
//};
//
//trait_as<int*, dummy, int> int_ptr_iterator{nullptr};
//
//
//struct node_base {
//  node_base() : m_next(0) {}
//  // Each node manages all of its tail nodes
//  virtual ~node_base() { delete m_next; }
//  // Access the rest of the list
//  node_base* next() const { return m_next; }
//  // print to the stream
//  virtual void print(std::ostream& s) const = 0;
//  // double the value
//  virtual void double_me() = 0;
//  void append(node_base* p) {
//    if (m_next)
//      m_next->append(p);
//    else
//      m_next = p;
//  }
//
// private:
//  node_base* m_next;
//};
//inline std::ostream& operator<<(std::ostream& s, node_base const& n) {
//  n.print(s);
//  return s;
//}
//
//template <class T>
//struct node : node_base {
//  node(T x) : m_value(x) {}
//  void print(std::ostream& s) const override { s << this->m_value; }
//  void double_me() override { m_value += m_value; }
//
// private:
//  T m_value;
//};
//}  // namespace
//
//namespace anyxx26 {
//template <typename ValueType, typename Reference>
//struct forward_iterator<model_map, node_base*, ValueType, Reference> {
//    static node_base* op_plus_plus(node_base*& x) { return x = x->next(); }
//};
//
//
//using op_star_t = anyxx26::const_trait_invoke_op_star<node_base*>;
//using op_star_t1 = typename [:substitute(^^const_trait_invoke_op_star, { ^^node_base* }):];
//consteval{
////    auto op_star = substitute(^^const_trait_invoke_op_star, {^^node_base*});
//    //auto params = make_find_candidate_in_target_params(^^input_iterator<declaration, declaration, node_base>::op_star, ^^node_base);
//    //constexpr auto implementation_member = get_implementation_member
//    //        (fitting_dyn_parameter_type(^^declaration)
//    //            , ^^input_iterator<declaration, node_base*, node_base>, ^^input_iterator<declaration, declaration, node_base, node_base&>::operator==);
//    //decltype(auto) y = input_iterator<declaration, node_base*, node_base>::op_star(nullptr);
//
//    //constexpr auto trait = ^^input_iterator<declaration, declaration, node_base, node_base&>;
//    //static_assert(
//    //    implementation_member ==
//    //    ^^input_iterator<declaration, node_base*, node_base>::operator==);
//    //static_assert(implementation_member != std::meta::info{});
//
//    //constexpr auto implementation_member1 = get_implementation_member
//    //    (fitting_dyn_parameter_type(^^declaration)
//    //    , ^^input_iterator<declaration, declaration, node_base>, ^^input_iterator<model_map, declaration, node_base, node_base&>::operator==);
//    //static_assert(implementation_member1 != std::meta::info{});
//    //static_assert(
//    //    implementation_member1 ==
//    //    ^^input_iterator<declaration, declaration, node_base>::operator==);
//    //constexpr auto implementation_member2 = get_implementation_member
//    //    (fitting_dyn_parameter_type(^^declaration)
//    //    , ^^ input_iterator<declaration, declaration, node_base>, ^^input_iterator<declaration, declaration, node_base, node_base&>::op_star);
//    //static_assert(implementation_member2 != std::meta::info{});
//    //static_assert(
//    //    implementation_member2 ==
//    //    ^^input_iterator<declaration, declaration, node_base>::op_star);
//    //constexpr auto args = std::define_static_array(template_arguments_of(trait) | std::views::drop(2));
//    //constexpr auto implementation_member3 = find_function_impl(^^input_iterator<declaration, declaration, node_base, node_base&>::operator==, ^^input_iterator, ^^node_base*, args);
//    //static_assert(implementation_member3 == ^^input_iterator<, node_base*, node_base, node_base&>::operator==);
//
//}
//
//}  // namespace anyxx26
//namespace {
//using node_iterator = anyxx26::using_<node_base*>::as<anyxx26::forward_iterator, node_base>;
//
//
//
////node_base& test(node_iterator& n){
////
////
////    auto& x = n.op_star();
////    return *n; 
////
////}
//static_assert(std::forward_iterator<node_iterator>);
//
//struct node_range : public std::ranges::subrange<node_iterator> {
//  node_range(node_base* node)
//      : std::ranges::subrange<node_iterator>(node_iterator(node),
//                                             node_iterator()) {}
//}
//
//}  // namespace
//
////TEST_CASE("example 3 iterator adaptor ") {
////  static_assert(std::ranges::input_range<std::ranges::subrange<node_iterator>>);
////  std::unique_ptr<node<int>> nodes(new node<int>(42));
////  nodes->append(new node<std::string>("is greater than:"));
////  nodes->append(new node<int>(13));
////
////  auto n = nodes.get();
////  auto r = std::ranges::subrange<node_iterator>(node_iterator(n), node_iterator());
////  std::stringstream out1;
////  std::ranges::copy(n, std::ostream_iterator<node_base>(out1, " "));
////  CHECK(out1.str() == "42 is greater than: 13 ");
////  std::for_each(node_iterator(n), node_iterator(),
////                [](auto& node) { node.double_me(); });
////  std::stringstream out2;
////  auto cr = node_const_range(n);
////  std::ranges::copy(cr, std::ostream_iterator<node_base>(out2, "/"));
////  CHECK(out2.str() == "84/is greater than:is greater than:/26/");
////}