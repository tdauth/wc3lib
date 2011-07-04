function toggleVis(elemid) {
	elem = document.getElementById(elemid);
	vis = elem.style.getPropertyValue('visibility');
	vis = elem.style.getPropertyValue('display');
	if (vis != 'none') {
		elem.style.setProperty('display', 'none', 'important');
	} else {
		elem.style.setProperty('display', '', 'important');
	}
}
