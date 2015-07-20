#include "io/pack.h"

typedef boost::shared_ptr<Pack> PackPtr;
uint32_t Pack::current_id = 1;

bool operator==(const PackPtr &lhs, const PackPtr &rhs)
{
	return (lhs->GetId() == rhs->GetId());
}

bool operator!=(const PackPtr &lhs, const PackPtr &rhs)
{
	return !(lhs->GetId() == rhs->GetId());
}