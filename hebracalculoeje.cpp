    #include "hebracalculoeje.h"

hebraCalculoEje::hebraCalculoEje(QObject *parent)
: QThread(parent)
{
    operacion = 0;
    setTerminationEnabled(true);
    padre = parent;
}

void hebraCalculoEje::setParametros(int * parametros){
    limiteVoxelesInferior = parametros[0];
    limiteVoxelesIntermedio = parametros[1]/100.;
    porcentajeDeseado = parametros[2]/100.;
    limiteVoxelesEje = parametros[3]/100.;
    limiteIteraciones = parametros[4];
    amplitudVoxels = parametros[5];
    amplitudMinima = parametros[6];
    calculoVoxels = parametros[7];
    calculoEje = parametros[8];
}

void hebraCalculoEje::insertarDatos(vcg::GlTrimesh<CMesh> glWrap2){
    glWrap = glWrap2;
}

void hebraCalculoEje::insertarVoxels(QMultiMap<int,Voxel> voxelsInicial){
    voxels = voxelsInicial;
}

void hebraCalculoEje::inicializarEje(){
    if(calculoVoxels){
        ObtenerEjeSinNormales();
    }

    if(calculoEje){
        RANSAC();
    }
}

void hebraCalculoEje::ObtenerEjeSinNormales(){
    emit IniciarBarra();
    float thetaActual, fiActual;
    float theta, fi;
    InicializarVoxels();
    voxels.clear();
    Voxel aux;
    CMesh::FaceIterator faceIterator=glWrap.m->face.begin();
    int numCaras = glWrap.m->face.size();
    vcg::Line3f nueva_normal;
    int contando = 0;
    int caraActual = 0;
    int maxContador = -1;
    while(faceIterator!=glWrap.m->face.end()){
        emit AvanzarBarra((int)(30.*caraActual/numCaras));
        nueva_normal.SetOrigin(vcg::Barycenter(*faceIterator));
        nueva_normal.SetDirection(vcg::Normal(*faceIterator).Normalize());
        normales.append(nueva_normal);
        orientacionNormales.insert(nueva_normal.Direction(),nueva_normal.Direction());
        fi = atan2(nueva_normal.Direction().X(),nueva_normal.Direction().Y());
        theta = sqrt(nueva_normal.Direction().X()*nueva_normal.Direction().X()+
                    nueva_normal.Direction().Y()*nueva_normal.Direction().Y()+
                    nueva_normal.Direction().Z()*nueva_normal.Direction().Z());
        theta = nueva_normal.Direction().Z()/theta;
        theta = theta<0?-acos(-theta):acos(theta);
        fi = fi+PI;
        fi = 180.*fi/PI;
        theta = theta+PI;
        theta = 180.*theta/PI;
        //Incrementar orientacionNormales[alfa][beta]
        contadorNormales[(int)theta][(int)fi]++;
        if(contadorNormales[(int)theta][(int)fi] > maxContador){
            maxContador = contadorNormales[(int)theta][(int)fi];
            thetaGlobal = (int) theta;
            fiGlobal = (int) fi;
        }
        ++faceIterator;
        ++contando;
        ++caraActual;
    }
    emit setStatusBar(tr("Normales recorridas"));

    contarClusterEje = (maxContador > 20);

    caraActual = 0;
    if(contarClusterEje){
        foreach(vcg::Line3f normal, normales){
            emit AvanzarBarra((int)(30+50.*caraActual/numCaras));
            fiActual = atan2(normal.Direction().X(),normal.Direction().Y());
            thetaActual = sqrt(normal.Direction().X()*normal.Direction().X()+
                        normal.Direction().Y()*normal.Direction().Y()+
                        normal.Direction().Z()*normal.Direction().Z());
            thetaActual = normal.Direction().Z()/thetaActual;
            thetaActual = thetaActual<0?-acos(-thetaActual):acos(thetaActual);
            fiActual = fiActual+PI;
            fiActual = (int) (180.*fiActual/PI);
            thetaActual = thetaActual+PI;
            thetaActual = (int) (180.*thetaActual/PI);
            if(!(fiActual == fiGlobal && thetaActual == thetaGlobal))
                ObtenerVoxelsNormal(normal,caraActual);
            caraActual++;
        }
    }else
        foreach(vcg::Line3f normal, normales){
            emit AvanzarBarra((int)(30+50.*caraActual/numCaras));
            ObtenerVoxelsNormal(normal,caraActual);
            caraActual++;
        }


    emit setStatusBar(tr("Normales calculadas"));
    vcg::Point3f punto;
    float dimension;
    maxInterseccionVoxel = 0;
    int m , mMax = anchoVoxels*anchoVoxels*anchoVoxels;
    for(int i=0; i<anchoVoxels; ++i)
        for(int j=0; j<anchoVoxels; ++j)
            for(int k=0; k<anchoVoxels; ++k){
                    if(voxels2[i][j][k]>limiteVoxelesInferior){
                        aux = Voxel(vcg::Point3i(i,j,k));
                        aux.asignarContador(voxels2[i][j][k]);
                        punto.X() = (i - valorMedio)*nodoRaiz.X();
                        punto.Y() = (j - valorMedio)*nodoRaiz.Y();
                        punto.Z() = (k - valorMedio)*nodoRaiz.Z();
                        dimension = sqrt(punto.X()*punto.X()+
                                         punto.Y()*punto.Y()+
                                         punto.Z()*punto.Z());
                        if(dimension > amplitudMinima * distanciaMinima/2.)
                            voxels.insert(voxels2[i][j][k],aux);
                        if(aux.contador() >= maxInterseccionVoxel) maxInterseccionVoxel = aux.contador();
                        emit AvanzarBarra((int)(80+20.*m/mMax));
                        m++;
                    }
                }
    emit AcabarBarra();
    emit setStatusBar(tr("Intersecciones calculadas."));
}

void hebraCalculoEje::InicializarVoxels(){
    BB = vcg::Point3f(glWrap.m->bbox.Dim() * 10);
    BBsmall = vcg::Point3f(glWrap.m->bbox.Dim());
    distanciaMinima = 0.5*glWrap.m->bbox.MinDim();
    posicionGlobal = glWrap.m->bbox.Center();
    nivelVoxels = 9;
    anchoVoxels = pow(2,nivelVoxels);
    voxels.clear();
    valorMedio = (int) anchoVoxels / 2;

    nodoRaiz = vcg::Point3f (2*BB.X()/anchoVoxels,2*BB.Y()/anchoVoxels,2*BB.Z()/anchoVoxels);
    maxInterseccionVoxel = 0;
    if (antiguoTamanioVoxel != -1){
        for(int i=0; i<antiguoTamanioVoxel; ++i){
            for(int j=0; j<antiguoTamanioVoxel; ++j){
                delete [] voxels2[i][j];
            }
            delete [] voxels2[i];
        }
        delete [] voxels2;
        voxels.clear();
    }
    voxels2 = new int ** [(int) anchoVoxels];
    for(int i=0; i<anchoVoxels; ++i){
        voxels2[i] = new int* [(int) anchoVoxels];
        for(int j=0; j<anchoVoxels; ++j){
            voxels2[i][j] = new int [(int) anchoVoxels];
            for(int k=0; k<anchoVoxels; ++k){
                voxels2[i][j][k] = 0;
            }
        }
    }
    for(int i=0; i< 360; ++i)
        for(int j=0; j< 360; ++j)
            contadorNormales[i][j]=0;

    antiguoTamanioVoxel = (int) anchoVoxels;
}

void hebraCalculoEje::ObtenerVoxelsNormal(vcg::Line3f normal, int caraActual){
    //Ver cual de las tres direcciones es mayor
    vcg::Point3f coordenada;
    vcg::Point3i indice, antiguoIndice, aux;
    vcg::Plane3f planoX, planoY, planoZ;
    bool sigue = true;
    float min, max;
    float pendienteYX = normal.Direction().Y()/normal.Direction().X();
    float pendienteZX = normal.Direction().Z()/normal.Direction().X();
    float pendienteXY = normal.Direction().X()/normal.Direction().Y();
    float pendienteZY = normal.Direction().Z()/normal.Direction().Y();
    float pendienteXZ = normal.Direction().X()/normal.Direction().Z();
    float pendienteYZ = normal.Direction().Y()/normal.Direction().Z();
    float anchoVoxelsBBX = anchoVoxels/(2*BB.X());
    float anchoVoxelsBBY = anchoVoxels/(2*BB.Y());
    float anchoVoxelsBBZ = anchoVoxels/(2*BB.Z());
    float normalX = normal.Direction().X(), normalY = normal.Direction().Y(), normalZ = normal.Direction().Z();
    if(normalX < 0) normalX = -normalX;
    if(normalY < 0) normalY = -normalY;
    if(normalZ < 0) normalZ = -normalZ;
    if(normalX > normalY && normalX > normalZ)
        //La direccion X es la mayor
    {
        min = -BB.X();
        max = BB.X();
        if(normal.Direction().X() > 0){
            min = floor(normal.Origin().X()/nodoRaiz.X())*nodoRaiz.X();
        }else{
            max = floor(normal.Origin().X()/nodoRaiz.X())*nodoRaiz.X();
        }
        sigue = true;
        for(coordenada.X() = min; coordenada.X() < max & sigue; coordenada.X()+=nodoRaiz.X()){
            coordenada.Y() = normal.Origin().Y() + (coordenada.X()-normal.Origin().X())*pendienteYX - posicionGlobal.Y();
            coordenada.Z() = normal.Origin().Z() + (coordenada.X()-normal.Origin().X())*pendienteZX - posicionGlobal.Z();
            if(coordenada.Y() < BB.Y() && coordenada.Y() > -BB.Y() && coordenada.Z() > -BB.Z() && coordenada.Z() < BB.Z()){
                indice.X() = (int) floor((coordenada.X() + BB.X())*anchoVoxelsBBX);
                indice.Y() = (int) floor((coordenada.Y() + BB.Y())*anchoVoxelsBBY);
                indice.Z() = (int) floor((coordenada.Z() + BB.Z())*anchoVoxelsBBZ);
                if(indice.Y() > 0 && indice.Y() < anchoVoxels && indice.Z() > 0 && indice.Z() < anchoVoxels){
                    voxels2[indice.X()][indice.Y()][indice.Z()]++;
                }
            }else{
                sigue = false;
            }
        }
    }else if(normalY > normalZ)
        //La direccion Y es la mayor
    {
        min = -BB.Y();
        max = BB.Y();
        if(normal.Direction().Y() > 0){
            min = floor(normal.Origin().Y()/nodoRaiz.Y())*nodoRaiz.Y();
        }else{
            max = floor(normal.Origin().Y()/nodoRaiz.Y())*nodoRaiz.Y();
        }
        sigue = true;
        for(coordenada.Y() = min; coordenada.Y() < max & sigue; coordenada.Y()+=nodoRaiz.Y()){
            coordenada.X() = normal.Origin().X() + (coordenada.Y()-normal.Origin().Y())*pendienteXY - posicionGlobal.X();
            coordenada.Z() = normal.Origin().Z() + (coordenada.Y()-normal.Origin().Y())*pendienteZY - posicionGlobal.Z();
            if(coordenada.X() < BB.X() && coordenada.X() > -BB.X() && coordenada.Z() > -BB.Z() && coordenada.Z() < BB.Z()){
                indice.X() = (int) floor((coordenada.X() + BB.X())*anchoVoxelsBBX);
                indice.Y() = (int) floor((coordenada.Y() + BB.Y())*anchoVoxelsBBY);
                indice.Z() = (int) floor((coordenada.Z() + BB.Z())*anchoVoxelsBBZ);
                if(indice.X() > 0 && indice.X() < anchoVoxels && indice.Z() > 0 && indice.Z() < anchoVoxels){
                   voxels2[indice.X()][indice.Y()][indice.Z()]++;
                }
            }else{
                sigue = false;
            }
        }
    }else
        //La direccion Z es la mayor
    {
        min = -BB.Z();
        max = BB.Z();
        if(normal.Direction().Z() > 0){
            min = floor(normal.Origin().Z()/nodoRaiz.Z())*nodoRaiz.Z();
        }else{
            max = floor(normal.Origin().Z()/nodoRaiz.Z())*nodoRaiz.Z();
        }
        sigue = true;
        for(coordenada.Z() = min; coordenada.Z() < max & sigue; coordenada.Z()+=nodoRaiz.Z()){
            coordenada.Y() = normal.Origin().Y() + (coordenada.Z()-normal.Origin().Z())*pendienteYZ - posicionGlobal.Y();
            coordenada.X() = normal.Origin().X() + (coordenada.Z()-normal.Origin().Z())*pendienteXZ - posicionGlobal.X();
            if(coordenada.Y() < BB.Y() && coordenada.Y() > -BB.Y() && coordenada.X() > -BB.X() && coordenada.X() < BB.X()){
                indice.X() = (int) floor((coordenada.X() + BB.X())*anchoVoxelsBBX);
                indice.Y() = (int) floor((coordenada.Y() + BB.Y())*anchoVoxelsBBY);
                indice.Z() = (int) floor((coordenada.Z() + BB.Z())*anchoVoxelsBBZ);
                if(indice.Y() > 0 && indice.Y() < anchoVoxels && indice.X() > 0 && indice.X() < anchoVoxels){
                    voxels2[indice.X()][indice.Y()][indice.Z()]++;
                }
            }else{
                sigue = false;
            }
        }
    }
}


void hebraCalculoEje::RANSAC(){
    inicializarSemilla();
  //  emit Imprimir("\nRealizando Ransac " + QString::number(porcentajeDeseado));
    int iteraciones = 0;
    float porcentaje, distancia;
    distancia =  amplitudVoxels*sqrt(nodoRaiz.X()*nodoRaiz.X()+
                     nodoRaiz.Y()*nodoRaiz.Y()+
                     nodoRaiz.Z()*nodoRaiz.Z());
    bool seguir = true;
    vcg::Point3f punto1,punto2,direccion;
    vcg::Line3f rectaPrueba;
    emit setStatusBar(tr("Realizando algoritmo RANSAC"));
    emit IniciarBarra();
    while(iteraciones < limiteIteraciones && seguir){
        emit AvanzarBarra((int)(100.*iteraciones/limiteIteraciones));
        if(contarClusterEje)
            SeleccionaPuntosCluster(punto1,punto2);
        else
            SeleccionaPuntos(punto1,punto2);
        direccion.X() = punto2.X() - punto1.X();
        direccion.Y() = punto2.Y() - punto1.Y();
        direccion.Z() = punto2.Z() - punto1.Z();
        rectaPrueba.Set(punto1,direccion);
        ComprobarDistancia(rectaPrueba,distancia,(int) maxInterseccionVoxel*(limiteVoxelesEje-10));
        ++iteraciones;
        porcentaje = (1.*voxelsDentro.size()) / (1.*(voxelsDentro.size() + voxelsFuera.size()));
        if(porcentaje > porcentajeDeseado) seguir = false;
        emit Imprimir("\t" + QString::number(porcentaje));
    }
    ejeFinal.Set(punto1,direccion);

    //emit Imprimir(tr("\nACABANDO RANSAC"));
    emit setStatusBar(tr("Eje calculado, para seguir ejecutando RANSAC pulse CONTINUAR."));
    emit AcabarBarra();
}

void hebraCalculoEje::SeleccionaPuntos(vcg::Point3f &punto1, vcg::Point3f & punto2){
    float seleccion1 = 1.*qrand()/RAND_MAX, seleccion2 = 1.*qrand()/RAND_MAX;
    int seleccionado1, seleccionado2 = (int) seleccion2*voxels.size();
    QMultiMap<int,Voxel>::iterator it1,it2;

    seleccionado1 = (int)(maxInterseccionVoxel*limiteVoxelesIntermedio + (seleccion1*(maxInterseccionVoxel*(1-limiteVoxelesIntermedio))));
    it1 = voxels.upperBound(seleccionado1);
    punto1.X() = (it1->posicion.X() - valorMedio)*nodoRaiz.X();
    punto1.Y() = (it1->posicion.Y() - valorMedio)*nodoRaiz.Y();
    punto1.Z() = (it1->posicion.Z() - valorMedio)*nodoRaiz.Z();

    while(seleccion1 == seleccion2){
        seleccion2 = 1.*qrand()/RAND_MAX;
    }
    seleccionado2 = (int)(maxInterseccionVoxel*limiteVoxelesIntermedio + (seleccion2*(maxInterseccionVoxel*(1-limiteVoxelesIntermedio))));

    it2 = voxels.upperBound(seleccionado2);
    if(it1 == it2) it2++;
    punto2.X() = (it2->posicion.X() - valorMedio)*nodoRaiz.X();
    punto2.Y() = (it2->posicion.Y() - valorMedio)*nodoRaiz.Y();
    punto2.Z() = (it2->posicion.Z() - valorMedio)*nodoRaiz.Z();
   // emit Imprimir("\nPuntos seleccionados: " + QString::number(seleccion1) + " " + QString::number(seleccion2));
   // emit Imprimir("\nPuntos seleccionados: " + QString::number(seleccionado1) + " " + QString::number(seleccionado2));
   // emit Imprimir("\nPuntos seleccionados: " + QString::number(it1->posicion.X()) + " " + QString::number(it2->posicion.X()));
   // emit Imprimir("\nPuntos seleccionados: " + QString::number(it1->posicion.Y()) + " " + QString::number(it2->posicion.Y()));
   // emit Imprimir("\nPuntos seleccionados: " + QString::number(it1->posicion.Z()) + " " + QString::number(it2->posicion.Z()));
   // emit Imprimir(tr("\n"));
}

void hebraCalculoEje::SeleccionaPuntosCluster(vcg::Point3f &punto1, vcg::Point3f & punto2){
    float seleccion1 = 1.*qrand()/RAND_MAX;
    int seleccionado1;
    float fi = fiGlobal, theta = thetaGlobal;
    QMultiMap<int,Voxel>::iterator it1;

    seleccion1 = 1.*qrand()/RAND_MAX;
    seleccionado1 = (int)(maxInterseccionVoxel*limiteVoxelesIntermedio + (seleccion1*(maxInterseccionVoxel*(1-limiteVoxelesIntermedio))));
    it1 = voxels.upperBound(seleccionado1);
    punto1.X() = (it1->posicion.X() - valorMedio)*nodoRaiz.X();
    punto1.Y() = (it1->posicion.Y() - valorMedio)*nodoRaiz.Y();
    punto1.Z() = (it1->posicion.Z() - valorMedio)*nodoRaiz.Z();

    vcg::Point3f direccion;
    fi = PI*fi/180.;
    fi = fi - PI;
    theta = PI*theta/180.;
    theta = theta + PI/2.;
    direccion.X() = sin(vcg::math::ToRad(fi))*cos(vcg::math::ToRad(theta));
    direccion.Y() = sin(vcg::math::ToRad(fi))*cos(vcg::math::ToRad(theta));
    direccion.Z() = cos(vcg::math::ToRad(fi));
    punto2 = punto1 + direccion;
}

float hebraCalculoEje::ComprobarDistancia(vcg::Line3f rectaPrueba, float distanciaMaxima, int filtro){
    float dist;
    vcg::Point3f puntoAux;
    voxelsDentro.clear();
    voxelsFuera.clear();
    int num=0;
    float distanciaMedia = 0;
    foreach(Voxel voxel, voxels){
        if(voxel.contador() > filtro){
            puntoAux.X() = (voxel.posicion.X() - valorMedio)*nodoRaiz.X();
            puntoAux.Y() = (voxel.posicion.Y() - valorMedio)*nodoRaiz.Y();
            puntoAux.Z() = (voxel.posicion.Z() - valorMedio)*nodoRaiz.Z();
            dist = vcg::Distance<float,false>(rectaPrueba,puntoAux);
            distanciaMedia += dist;
            num ++;
            if (dist < distanciaMaxima)
                voxelsDentro.append(voxel);
            else
                voxelsFuera.append(voxel);
        }
    }
    return distanciaMedia /= num;
}

void hebraCalculoEje::run(){
    if(operacion == 0)
        inicializarEje();
    if(operacion == 1)
        RANSAC();
    emit finalizaHebra(voxels,
                       valorMedio,
                       nodoRaiz,
                       maxInterseccionVoxel,
                       ejeFinal);
}
