
inline GenvisPreprocAction::Preproc::Preproc (genvis::OSGCache& cache)
 : _cache(cache)
{
}
inline void     GenvisPreprocAction::Preproc::pushMaterial (const MaterialPtr& m)
{
   _materials.push(m);
}
inline void     GenvisPreprocAction::Preproc::popMaterial  ()
{
   _materials.pop();
}
inline const GenvisPreprocAction::Preproc::MaterialStack& 
GenvisPreprocAction::Preproc::getMaterials () const
{
   return _materials;
}

inline genvis::OSGCache& GenvisPreprocAction::getCache ()
{
   return genvis::OSGCache::the();
}
