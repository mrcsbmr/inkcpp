#pragma once

#include "system.h"
#include "array.h"

#ifdef INKCPP_ENABLE_STL
#include <iosfwd>
#endif

namespace ink::internal {
	class header;
}
namespace ink::runtime::internal
{
	// TODO: move to utils
	// memory segments
	// @param bits size in bits
	// @param size segment size in bytes
	constexpr int segmentsFromBits(int bits, int size) {
		size *= 8;
		return bits / size + (bits % size ? 1 : 0);
	}

	/// managed all list entries and list metadata
	class list_table
	{
		using data_t = int;
		enum class state : char {
			unused,
			used,
			permanent,
			empty
		};

	public:
		/// handle to acces a list
		struct list{
			constexpr explicit list(int id) : lid{id} {}
			int lid; ///< id of list to handle
		};

		/// creates an empty list
		list create();

		/// zeros all usage values
		void clear_usage();

		/// mark list as used
		void mark_used(list);

		/// delete unused lists
		void gc();


		// function to setup list_table
		list create_permament();
		list& add_inplace(list& lh, list_flag rh);

		// parse binary list meta data
		list_table(const char* data, const ink::internal::header&);
		size_t stringLen(const list_flag& e) const;
		const char* toString(const list_flag& e) const;

		/** returns len of string representation of list */
		size_t stringLen(const list& l) const;

		/** converts list to string representation
		 * @param out char array with minimu size of stringLen(l)
		 * @param l list to stringify
		 * @return pointer to end of insierted string
		 */
		char* toString(char* out, const list& l) const;
		
		list add(list lh, list rh);
		list add(list l, int i);
		list add(list lh ,list_flag rh);
		list sub(list lh, list rh);
		list sub(list l, int i);
		list sub(list lh, list_flag rh);
		int count(list l);
		list_flag min(list l);
		list_flag max(list l);
		list_flag lrnd(list l);
		list all(list l);
		list invert(list l);
		bool less(list lh, list rh);
		bool greater(list lh, list rh);
		bool equal(list lh, list rh);
		bool not_equal(list lh, list rh){ return equal(lh, rh); }
		bool greater_equal(list lh, list rh);
		bool less_equal(list lh, list rh);
		operator bool (){
			return _valid;
		}

	private:
		void copy_lists(const data_t* src, data_t* dst);
		static constexpr int bits_per_data = sizeof(data_t) * 8;
		int listBegin(int lid) const {
			return lid == 0 ? 0 : _list_end[lid-1];
		}
		const data_t* getPtr(int eid) const {
			return _data.begin() + _entrySize * eid;
		}
		data_t* getPtr(int eid)	 {
			return _data.begin() + _entrySize * eid;
		}
		int numFlags() const {
			return _flag_names.size();
			// return _list_end.end()[-1]; TODO: 
		}
		int numLists() const {
			return _list_end.size();
		}
		bool getBit(const data_t* data, int id) const {
			return data[id / bits_per_data] &
				(0x01 << (bits_per_data - 1 - (id % bits_per_data)));
		}
		void setBit(data_t* data, int id, bool value = true) {
			data_t mask = 0x01 << (bits_per_data-1 - (id % bits_per_data));
			if (value) {
				data[id/bits_per_data] |= mask;
			} else {
				data[id/bits_per_data] &= ~mask;
			}
		}
		bool hasList(const data_t* data, int lid) const {
			return getBit(data, lid);
		}
		void setList(data_t* data, int lid, bool value = true) {
			setBit(data, lid, value);
		}
		bool hasFlag(const data_t* data, int fid) const {
			return getBit(data, fid + numLists());
		}
		void setFlag(data_t* data, int fid, bool value = true) {
			setBit(data, fid + numLists(), value);
		}
		int toFid(list_flag e) const;
		auto flagStartMask() const {
			struct { int segment; data_t mask; }
			res {
				numLists() / bits_per_data,
				~static_cast<data_t>(0) >> (numLists() % bits_per_data)};
			return res;
		}

		template<typename T, int config>
		using managed_array = managed_array<T, config < 0, abs(config)>;

		static constexpr int maxMemorySize = 
			(config::maxListTypes < 0
			 || config::maxFlags < 0
			 || config::maxLists < 0
			 ? -1
			 : 1) *
			segmentsFromBits(
					abs(config::maxListTypes)
					+ abs(config::maxFlags),
					sizeof(data_t)
			) * abs(config::maxLists);

		int _entrySize; ///< entry size in data_t 
		// entries (created lists)
		managed_array<data_t, maxMemorySize> _data;
		managed_array<state, config::maxLists> _entry_state;

		// defined list (meta data)
		managed_array<int, config::maxListTypes> _list_end;
		managed_array<const char*,config::maxFlags> _flag_names;

		bool _valid;
	public:
		class named_flag_itr {
			using list_size_t = decltype(list_table::_list_end);
			using flag_name_t = decltype(list_table::_flag_names);
			const list_size_t* _list_end = nullptr;
			const flag_name_t* _flag_names = nullptr;
			struct {
				list_flag flag;
				const char* name;
			} _pos;
		public:
			bool operator!=(const named_flag_itr& o) const  {
				return _pos.flag != o._pos.flag;
			}
			named_flag_itr() = default;
			named_flag_itr(const list_size_t& ls, const flag_name_t& fn)
				: _list_end{&ls}, _flag_names{&fn}, _pos{{0,0},fn[0]}{
					while(_pos.name == nullptr) {
						++_pos.flag.flag;
						_pos.name = (*_flag_names)[_pos.flag.flag];
					}
				}
			const auto* operator->() const { return &_pos; }
			const auto& operator*() const { return _pos; }
			const named_flag_itr& operator++() {
				if(_pos.flag != null_flag) return *this;
add:
				++_pos.flag.flag;
				if(_pos.flag.flag == 
						(*_list_end)[_pos.flag.list_id]
						- (_pos.flag.list_id > 0 ? (*_list_end)[_pos.flag.list_id] : 0)) {
					++_pos.flag.list_id;
					_pos.flag.flag = 0;
					if(_pos.flag.list_id == _flag_names->size()) {
						_pos.flag = null_flag;
						_pos.name = nullptr;
						return *this;
					}
				}
				_pos.name = (*_flag_names)[(*_list_end)[_pos.flag.list_id] + _pos.flag.flag];
				if (_pos.name == nullptr) {
					goto add;
				}
				return *this;
			}
		};
		auto named_flags() const {
			struct {
					named_flag_itr _begin;
					named_flag_itr _end;
					named_flag_itr begin() const {
						return _begin;
					}
					named_flag_itr end() const {
						return _end;
					}
			} res {
				named_flag_itr(_list_end, _flag_names),
				named_flag_itr()};
			return res;
		}
	};

#ifdef INK_ENABLE_STL
	std::ostream& operator<<(std::ostream&, const list_table::list&);
	std::ostream& operator<<(std::ostream&, const list_flag&);
#endif
}
