#include "OSGGVBVolDistance.h"

USING_GENVIS_NAMESPACE

// explicit instantiations
template class BVolDistanceBase<OpenSGTraits, EuclideanMetric>;
template class BVolDistanceBase<OpenSGTraits, EuclideanMetricPlane>;
template class BVolDistanceBase<OpenSGTraits, EuclideanMetricHalfspace>;

template <class BasicTraits, class Metric>
Real BVolDistanceBase<BasicTraits,Metric>::s_maxDistance = infinity;

template <class BasicTraits, class Metric>
typename BVolDistanceBase<BasicTraits,Metric>::MetricType 
BVolDistanceBase<BasicTraits,Metric>::s_metric;
