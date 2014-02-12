/* Nicolas Lefèvre */

/* A faire

Integrer le script dans le gestionnaire CTK
Ajouter un modificateur de peinture par passe, pour sauvegarder
forcer une passe si nb_faces > 5000
*/

rollout mon_roll "Progression AO -> Vertex" (
	label label_face "Face" pos:[5,5] width:150 height:16
	label label_face_indice "/" pos:[210,5] width:80 height:16
	progressBar prog_face "face" pos:[50,5] width:150 height:16
	
	label label_passe "Passe" pos:[5,20] width:150 height:16
	label label_passe_indice "/" pos:[210,20] width:80 height:16
	progressBar prog_passe "passe" pos:[50,20] width:150 height:16
	
	label label_objet "Objet" pos:[5,35] width:150 height:16
	label label_objet_indice "/" pos:[210,35] width:80 height:16
	progressBar prog_objet "objet" pos:[50,35] width:150 height:16
)


function CreateGui = (
	createDialog  mon_roll  290 60
	)

function CalculSurfaceDisque obj sommet = (
	s = 0
	tab_faces = polyop.getFacesUsingVert obj sommet as array
	for i=1 to tab_faces.count do s = s + polyop.getFaceArea obj (tab_faces[i])
	if tab_faces.count == 0 then s = 1
	else s = s/tab_faces.count
	s
)

function CreerTableSurface obj = (
	tab_result = #()
	for i=1 to polyop.getNumVerts obj do append tab_result (CalculSurfaceDisque obj i)
	tab_result
)

function CreerTablePosition obj = (
	tab_result = #()
	for i=1 to polyop.getNumVerts obj do append tab_result (polyop.getVert obj i)
	tab_result
)



function CalculNormaleSommet obj sommet = (
	--A verifier
	normale = [0,0,0]
	tab_faces = polyop.getFacesUsingVert  obj sommet as array
	for i=1 to tab_faces.count do normale = normale + polyop.getFaceNormal obj (tab_faces[i])
	
	normale = normalize normale
	
	normale
)

function CreerTableNormale obj = (
	tab_result = #()
	for i=1 to polyop.getNumVerts obj do append tab_result (CalculNormaleSommet obj i)
	tab_result
)



function FormFactor indiceEmetteur indiceReceveur tab_normale tab_surface tab_position RCVToEMT= (
	
	valeur = 0;
	dist =  length RcvToEmt
	--if (dist < 1 * tab_surface[indiceEmetteur]) then ( -- remplacer par une distance...
	
	Normalize RcvToEmt
	cosE = dot (RcvToEmt) (tab_normale[indiceEmetteur])
	cosR = dot (RcvToEmt) (tab_normale[indiceReceveur])
		
		--print (cosE * cosR * tab_surface[indiceEmetteur] / (Pi * dist * dist * tab_surface[indiceEmetteur]))
	if (cosE >= 0 and cosR >= 0) then valeur = cosE * cosR * tab_surface[indiceEmetteur] / (Pi * dist * dist + tab_surface[indiceEmetteur])
	
	valeur
)


function ClampOcclusion valeur = (
	--
	NouvelleOcclusion = exp (-valeur)
	--NouvelleOcclusion = valeur
    if (NouvelleOcclusion < 0) then NouvelleOcclusion = 0
    if (NouvelleOcclusion > 1) then NouvelleOcclusion = 1
	
    NouvelleOcclusion
)


function CreerOcclusion obj nbpasse dista= (
	print dista
	--100ms
	tab_normale = CreerTableNormale obj
	tab_surface = CreerTableSurface obj
	tab_position = CreerTablePosition obj
		
	tab_occlusion = #()
	
	for i = 1 to tab_normale.count do append tab_occlusion 1
	
	for passe = 1 to nbpasse do (
		
		mon_roll.prog_passe.value = 100.0*passe / nbpasse
		mon_roll.label_passe_indice.text = (passe as string) +"/"+ (nbpasse as string)
			
		--tt = timestamp() 
		for i = 1 to tab_normale.count do (
			mon_roll.prog_face.value = 100.0*i / tab_normale.count
			mon_roll.label_face_indice.text = (i as string) +"/"+ (tab_normale.count as string)
			occ_i =0
			for j=1 to tab_normale.count where i!=j do (
				RCVtoEMT = (tab_position[j] - tab_position[i])
				if ((length RCVtoEMT) < dista) then (
					valeur = FormFactor j i tab_normale tab_surface tab_position RCVtoEMT
					if (valeur !=0) then occ_i  = occ_i + valeur * tab_occlusion[j]
				)
			)
			occ_i = ClampOcclusion (occ_i)
			tab_occlusion[i] = occ_i
			
			
		)
		--tt2 =  timestamp() 
		--print ("duree : " + (tt2-tt) as string)
			
	)
	
	tab_occlusion
		
)

function LisserOcc tab_occ precision:20= (
	tab_result = #()
	
	--Certaines valeurs restent a 1 !
	for i=1 to tab_occ.count do (
		append tab_result ((ceil (tab_occ[i] *precision))/precision) -- indice ok !
	)
	tab_result
)

function selectIndice tab valeur = (
	tab_result = #()
	
	for j=1 to tab.count  do (
			
			if(tab[j] == valeur) then append tab_result j
		)
	tab_result	
)



function lissagePeinture obj tab_occ nb_lissage:1 precision:20= (
	if (nb_lissage<1) then return tab_occ
	
	tab_occ_new= #()
	--timest1 = timestamp() 
	
	for i=1 to polyop.getNumVerts obj do (
		tab_indice = #()
		faces = (polyop.getFacesUsingVert obj i) as array
		for f = 1 to faces.count do (
			--print ("Face :"+f as string)
			tab_vert = (polyop.getVertsUsingFace obj faces[f]) as array
			for k=1 to tab_vert.count do appendifunique tab_indice (tab_vert[k])
		)
		
		nouvelle_valeur =3*tab_occ[i]
		for j=1 to tab_indice.count do (
			nouvelle_valeur = nouvelle_valeur + tab_occ[tab_indice[j]]
		)
		nouvelle_valeur = nouvelle_valeur / (3+tab_indice.count)
		append tab_occ_new nouvelle_valeur
		
	)
	
	--timest2 =timestamp() 
	--print ("duree Lissage: " + (timest2-timest1) as string)
	
	tab_occ_new = LisserOcc tab_occ_new  precision:precision
	
	
	if (nb_lissage <=0) then return tab_occ
	if (nb_lissage == 1) then return tab_occ_new
		
	if(nb_lissage >1 ) then lissagePeinture obj tab_occ_new nb_lissage:(nb_lissage-1) precision:precision
	--tab_occ_new
)


function PeindreAO obj tab_occ precision:20 lisser:1= (
	tab_occ_lisse = lissagePeinture obj tab_occ nb_lissage:lisser
	
	for i=0 to precision do (
		p = (1.0*i/precision*1.0)
		tab_bit_array = selectIndice tab_occ_lisse p
		couleur = (Color (255  * p) (255  * p) (255  * p))
		polyOp.setVertColor obj 0 (tab_bit_array as bitarray) couleur	
	)
	
	tab_occ_lisse	
)


function PeintureAO obj nbpasse precisionLissage:20 dist:0.25 lisser:1=(
	--tt = timestamp() 
	with redraw off( 
		with undo off(
			tab_occ = CreerOcclusion obj nbpasse dist
			if (lisser >= 1 ) then tab_occ = PeindreAO obj tab_occ precision:precisionLissage lisser:lisser
			else PeindreAO obj tab_occ precision:precisionLissage lisser:0
		)
	)
	--tt2 =timestamp() 
	
	--print ("duree : " + (tt2-tt) as string)
	
	tab_occ
)



function PeindreObjet obj nbpasse precisionLissage:20 dist:0.25 lisser:1=(
		with redraw off( 
			with undo off(
				--converison en poly
				copie = copy obj
				converttopoly copie
				
				/* Optimisation */
				passe = nbpasse
				if(polyop.getnumverts copie > 3000) then  passe = 1
				
				
				PeintureAO copie passe precisionLissage:precisionLissage dist:dist lisser:lisser
				addmodifier obj (edit_poly())
				obj.modifiers[1].name = "PeintureSommet_poly"	
					
				channelInfo.CopyChannel copie 3 0
				channelInfo.PasteChannel obj 3 0
					
				obj.modifiers[1].name = "PeintureSommet_Mapping"	
					
				delete copie
			)
		)		
	) 

	
function PeindreSelection tab_obj nb_passe precisionLissage:20 dist:0.25 lisser:1 = (
	CreateGui()
	for i=1 to tab_obj.count do (
		mon_roll.prog_objet.value = 100.0*i/tab_obj.count
		mon_roll.label_objet_indice.text = tab_obj[i].name
		
		PeindreObjet tab_obj[i] nb_passe precisionLissage:precisionLissage dist:dist lisser:lisser
	)
	
	mon_roll.label_objet_indice.text = "Terminé !"
	mon_roll.label_face_indice.text = "Terminé !"	
	mon_roll.label_passe_indice.text = "Terminé !"
	DestroyDialog mon_roll
)

/* GUI */

function InitialiseGUI = (
	index_face = 1
	index_passe = 1
	index_objet = 1
)

function SupprimerPeinture = (
	for o in geometry where o.modifiers!=undefined do (
		for m=o.modifiers.count to 1 by -1 do(
			if (o.modifiers[m].name == "PeintureSommet_poly" or  o.modifiers[m].name == "PeintureSommet_Mapping") then deletemodifier o o.modifiers[m]
		)
	)
)






--tt = PeintureAO $ 1 dist:1.0
--lissagePeinture $ tt