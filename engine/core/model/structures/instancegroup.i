%module fife %{
#include "model/structures/instancegroup.h"
%}

%include "std_list.i"
%include "util/point.i"
%include "util/attributedclass.i"

namespace FIFE {
	class InstanceGroup;
}

namespace std {
	%template(InstanceGroupList) list<FIFE::InstanceGroup*>;
}

namespace FIFE {
	typedef std::list<InstanceGroup*> InstanceGroupList;
	typedef std::list<Instance*> InstanceList;

	class InstanceGroupManager {
		public:
			InstanceGroupManager();
			~InstanceGroupManager();

			void clear();
			InstanceGroup *addGroup( InstanceGroup *group );
			InstanceGroup *createGroup( InstanceGroup &group );
			void removeGroup( InstanceGroup *group );
			InstanceGroup *getGroup( const std::string &id, bool recursive = false ) const;
			bool isChild( InstanceGroup *group ) const;
			InstanceGroupList &getGroups();
	};

	class InstanceGroup : public InstanceGroupManager, public AttributedClass {
		public:
			InstanceGroup( const std::string &identifier, DoublePoint3D &offset, InstanceGroup *parent = NULL );
			~InstanceGroup();

			void addInstance( Instance *instance );
			void removeInstance( Instance *instance );
			InstanceList &getMembers();
			bool isMember( Instance *instance ) const;
			void setOffset( DoublePoint3D &offset );
			const DoublePoint3D getOffset() const;
	};
}
