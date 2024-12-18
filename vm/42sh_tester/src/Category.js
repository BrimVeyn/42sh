import React, {useState} from 'react';
import UnitTest from './UnitTest';
import Resume from './Resume';

function Category({ name, passed, total, inject }) {

	const categoryStyle = "w-1/4 flex flex-col justify-center text-center border-2 border-zinc-300 hover:bg-zinc-100 hover:text-blue-500 hover:scale-105 hover:shadow-lg transition-transform duration-200"

	return (
		<>
			<div className={categoryStyle} onClick={inject}>
					<p className="text-xs overflow-hidden" >{name} </p>
					<p> {passed} / {total} </p>
			</div>
		</>
	);
}

export default Category;
