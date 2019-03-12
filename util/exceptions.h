#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include <exception>
#include "../typedefs.h"

#define EXCEPTION(ClassName, Message) \
class ClassName : public detail::base_exception { \
public: \
	ClassName() : detail::base_exception(Message) { } \
}; \

namespace util {
	namespace detail {
		class base_exception : public std::exception {
		public:
			explicit base_exception(const char *msg) noexcept : m_msg{ msg } { }
			virtual ~base_exception() throw() { }

			virtual const char *what() const throw() {
				return m_msg;
			}
		protected:
			const char *m_msg;
		};
	} // namespace detail

	EXCEPTION(invalid_course_level, "Entered course level isn't valid.");
	EXCEPTION(bad_node_index, "Invalid node index for linked list.");
	EXCEPTION(failed_assignment_alloc, "Something's gone wrong when populating assignment details with those provided.");
	EXCEPTION(bad_weight_total, "Assignments' weight total != 100%");
} // namespace util

#endif EXCEPTIONS_H // EXCEPTIONS_H