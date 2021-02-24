#pragma once

namespace ink::runtime::internal {
	template<value_type ty>
	using is_numeric_t = typename std::enable_if<
		ty == value_type::int32
		|| ty == value_type::uint32
		|| ty == value_type::float32, void>::type;

	template<value_type ty>
	using is_integral_t = typename std::enable_if<
		ty == value_type::int32
		|| ty == value_type::uint32, void>::type;

	namespace casting {
		template<>
		constexpr value_type cast<value_type::int32, value_type::float32> = value_type::float32;
		template<value_type to>
		inline value::ret<to>::type numeric_cast(const value& v) {
			if (to == v.type()) { return v.get<to>(); }
			else {
				throw ink_exception("invalid numeric_cast!");
			}
		}

		template<>
		inline float numeric_cast<value_type::float32>(const value& v) {
			switch(v.type()) {
				case value_type::float32:
					return v.get<value_type::float32>();
				case value_type::int32:
					return static_cast<float>(v.get<value_type::int32>());
				default:
					throw ink_exception("invalid numeric_cast!");
			}
		}
	}

	template<value_type ty>
	class operation<Command::ADD, ty, is_numeric_t<ty>> : public operation_base<void> {
	public:
		using operation_base::operation_base;
		void operator()(eval_stack& stack, value* vals) {
			stack.push(value{}.set<ty>(
						casting::numeric_cast<ty>(vals[0]) +
						casting::numeric_cast<ty>(vals[1]) ));
		}
	};

	template<value_type ty>
	class operation<Command::SUBTRACT, ty, is_numeric_t<ty>> : public operation_base<void> {
	public:
		using operation_base::operation_base;
		void operator()(eval_stack& stack, value* vals) {
			stack.push(value{}.set<ty>(
						casting::numeric_cast<ty>(vals[0]) -
						casting::numeric_cast<ty>(vals[1]) ));
		}
	};

	template<value_type ty>
	class operation<Command::DIVIDE, ty, is_numeric_t<ty>> : public operation_base<void> {
	public:
		using operation_base::operation_base;
		void operator()(eval_stack& stack, value* vals) {
			stack.push(value{}.set<ty>(
						casting::numeric_cast<ty>(vals[0]) /
						casting::numeric_cast<ty>(vals[1]) ));
		}
	};

	template<value_type ty>
	class operation<Command::MULTIPLY, ty, is_numeric_t<ty>> : public operation_base<void> {
	public:
		using operation_base::operation_base;
		void operator()(eval_stack& stack, value* vals) {
			stack.push(value{}.set<ty>(
						casting::numeric_cast<ty>(vals[0]) *
						casting::numeric_cast<ty>(vals[1]) ));
		}
	};

	template<value_type ty>
	class operation<Command::MOD, ty, is_integral_t<ty>> : public operation_base<void> {
	public:
		using operation_base::operation_base;
		void operator()(eval_stack& stack, value* vals) {
			stack.push(value{}.set<ty>( vals[0].get<ty>() % vals[1].get<ty>() ));
		}
	};

	template<value_type ty>
	class operation<Command::IS_EQUAL, ty, is_numeric_t<ty>> : public operation_base<void> {
	public:
		using operation_base::operation_base;
		void operator()(eval_stack& stack, value* vals) {
			stack.push(value{}.set<value_type::boolean>(
				casting::numeric_cast<ty>(vals[0]) ==
				casting::numeric_cast<ty>(vals[1])
			));
		}
	};

	template<value_type ty>
	class operation<Command::GREATER_THAN, ty, is_numeric_t<ty>> : public operation_base<void> {
	public:
		using operation_base::operation_base;
		void operator()(eval_stack& stack, value* vals) {
			stack.push(value{}.set<value_type::boolean>(
				casting::numeric_cast<ty>(vals[0]) >
				casting::numeric_cast<ty>(vals[1])
			));
		}
	};


	template<value_type ty>
	class operation<Command::LESS_THAN, ty, is_numeric_t<ty>> : public operation_base<void> {
	public:
		using operation_base::operation_base;
		void operator()(eval_stack& stack, value* vals) {
			stack.push(value{}.set<value_type::boolean>(
				casting::numeric_cast<ty>(vals[0]) <
				casting::numeric_cast<ty>(vals[1])
			));
		}
	};

	template<value_type ty>
	class operation<Command::GREATER_THAN_EQUALS, ty, is_numeric_t<ty>> : public operation_base<void> {
	public:
		using operation_base::operation_base;
		void operator()(eval_stack& stack, value* vals) {
			stack.push(value{}.set<value_type::boolean>(
				casting::numeric_cast<ty>(vals[0]) >=
				casting::numeric_cast<ty>(vals[1])
			));
		}
	};


	template<value_type ty>
	class operation<Command::LESS_THAN_EQUALS, ty, is_numeric_t<ty>> : public operation_base<void> {
	public:
		using operation_base::operation_base;
		void operator()(eval_stack& stack, value* vals) {
			stack.push(value{}.set<value_type::boolean>(
				casting::numeric_cast<ty>(vals[0]) <=
				casting::numeric_cast<ty>(vals[1])
			));
		}
	};

	template<value_type ty>
	class operation<Command::NOT_EQUAL, ty, is_numeric_t<ty>> : public operation_base<void> {
	public:
		using operation_base::operation_base;
		void operator()(eval_stack& stack, value* vals) {
			stack.push(value{}.set<value_type::boolean>(
				casting::numeric_cast<ty>(vals[0]) !=
				casting::numeric_cast<ty>(vals[1])
			));
		}
	};

	template<value_type ty>
	class operation<Command::AND, ty, is_integral_t<ty>> : public operation_base<void> {
	public:
		using operation_base::operation_base;
		void operator()(eval_stack& stack, value* vals) {
			stack.push(value{}.set<value_type::boolean>( vals[0].get<ty>() && vals[1].get<ty>() ));
		}
	};

	template<value_type ty>
	class operation<Command::OR, ty, is_integral_t<ty>> : public operation_base<void> {
	public:
		using operation_base::operation_base;
		void operator()(eval_stack& stack, value* vals) {
			stack.push(value{}.set<value_type::boolean>( vals[0].get<ty>() || vals[1].get<ty>() ));
		}
	};

	template<value_type ty>
	class operation<Command::MIN, ty, is_numeric_t<ty>> : public operation_base<void> {
	public:
		using operation_base::operation_base;
		void operator()(eval_stack& stack, value* vals) {
			typename value::ret<ty>::type n[2] = {
				casting::numeric_cast<ty>(vals[0]),
				casting::numeric_cast<ty>(vals[1])
			};
			stack.push(value{}.set<ty>(n[0] < n[1] ? n[0] : n[1]));
		}
	};

	template<value_type ty>
	class operation<Command::MAX, ty, is_numeric_t<ty>> : public operation_base<void> {
	public:
		using operation_base::operation_base;
		void operator()(eval_stack& stack, value* vals) {
			typename value::ret<ty>::type n[2] = {
				casting::numeric_cast<ty>(vals[0]),
				casting::numeric_cast<ty>(vals[1])
			};
			stack.push(value{}.set<ty>(n[0] > n[1] ? n[0] : n[1]));
		}
	};

	template<value_type ty>
	class operation<Command::NOT, ty, is_integral_t<ty>> : public operation_base<void> {
	public:
		using operation_base::operation_base;
		void operator()(eval_stack& stack, value* vals) {
			stack.push(value{}.set<value_type::boolean>(!vals[0].get<ty>()));
		}
	};

	template<value_type ty>
	class operation<Command::NEGATE, ty,  is_numeric_t<ty>> : public operation_base<void> {
	public:
		using operation_base::operation_base;
		void operator()(eval_stack& stack, value* vals) {
			stack.push(value{}.set<ty>(-vals[0].get<ty>()));
		}
	};
}
