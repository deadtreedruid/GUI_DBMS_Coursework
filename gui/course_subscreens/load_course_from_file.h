#ifndef COURSE_LOAD_COURSE_FROM_FILE_H
#define COURSE_LOAD_COURSE_FROM_FILE_H
#include "..\screen.h"
#include "..\..\util\linked_list.h"
#include "..\..\course.h"

namespace pugi { class xml_node; }

namespace gui {
	namespace course_subscreens {
		class load_course_from_file : public screen {
			using list_type = util::linked_list<course>;
		public:
			load_course_from_file();
			void draw() override;
			void handle_event(sf::Event &) override;

		private:
			void setup_controls();
			void load();
			static list_type::node_t *load_from_node(pugi::xml_node &node, list_type *courses);
		};
	}
}

#endif