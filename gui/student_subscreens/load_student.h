#ifndef STUDENT_OPTIONS_LOAD_STUDENT_H
#define STUDENT_OPTIONS_LOAD_STUDENT_H
#include "..\screen.h"
#include "..\..\util\linked_list.h"
#include "..\..\student.h"

namespace pugi { class xml_node; }

namespace gui {
	namespace enrol_subscreens { class enrol_from_file; }

	namespace student_subscreens {
		class load_student : public screen {
			friend class enrol_subscreens::enrol_from_file;
			using list_type = util::linked_list<student>;
		public:
			load_student();
			void draw() override;
			void handle_event(sf::Event &) override;

		private:
			void setup_controls();
			void load();
			static list_type::node_t *load_from_node(pugi::xml_node &node, list_type *students, bool ignore_id_in_app_error = false);
		};
	}
}

#endif