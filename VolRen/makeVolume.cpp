NodePtr makeVolume( const char * datFile)
{  
    DVRVolumePtr         geo         = DVRVolume::create();
    DVRAppearancePtr     app         = DVRAppearance::create(); 
    DVRLookupTablePtr    lut         = DVRLookupTable::create();
    DVRVolumeTexturePtr  vol         = DVRVolumeTexture::create();
    DVRIsoSurfacePtr     iso         = DVRIsoSurface::create();

    //!! FIXME - This code can set up available texture memory
    const FieldContainerType & cType = geo->getType();
    const FieldDescription *   desc  = cType.findFieldDescription("brickStaticMemoryMB");
    Field *                    field = geo->getField(desc->getFieldId());
    SFUInt16 *   brickStaticMemoryMB = dynamic_cast<SFUInt16*>(field);
    if (brickStaticMemoryMB)
      brickStaticMemoryMB->setValue(4);

    // Load the 3D-image and store it in the volume texture attachment
    ImageP datImage = new Image();
    datImage->read(datFile);
    datImage->dump();
    beginEditCP(vol);
    vol->setImage(datImage);
    vol->setFileName(datFile);
    endEditCP(vol);

    // Attach the volume, a lookup table and an isosurface attachment
    beginEditCP(app, Node::AttachmentsFieldMask);
    app->addAttachment(vol);
    app->addAttachment(lut);
    app->addAttachment(iso);
    endEditCP  (app, Node::AttachmentsFieldMask);
    
    beginEditCP(geo);
    geo->setFileName(datFile);
    geo->setAppearance(app);
    geo->setShader(DVRSimpleLUTShader::create());
    endEditCP(geo);
      
    NodePtr newGeom = Node::create();
    beginEditCP(newGeom);
    newGeom->setCore( geo );
    endEditCP(newGeom);
    
    TransformPtr tr  = Transform::create();
    Matrix scale;
    beginEditCP(tr);
    scale.setScale(1.0/64, 1.0/64, 1.0/64);
    endEditCP(tr);
    tr->setMatrix(scale);
      
    NodePtr trn = Node::create();
    beginEditCP(trn);
    trn->setCore( tr );
    trn->addChild( newGeom );
    endEditCP(trn);

    return trn;
}
