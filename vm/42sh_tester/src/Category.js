import React, {useState} from 'react';
import UnitTest from './UnitTest';
import Resume from './Resume';

function Category({ name, inject }) {

	const categoryStyle = "text-center border-2 border-zinc-300 py-1 m-3 category hover:bg-zinc-100 hover:text-blue-500 hover:scale-105 hover:shadow-lg transition-transform duration-200"

	return (
		<>
			<div className={categoryStyle} onClick={inject}>
					<a>{name}</a>
			</div>
		</>
	);
}

export default Category;
