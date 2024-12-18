import React, {useState} from 'react';
import UnitTest from './UnitTest';
import Resume from './Resume';

function Category({ name, testsInfo, inject }) {

	const categoryStyle = "w-1/4 flex flex-col justify-center text-center border-2 border-zinc-300 hover:bg-gray-600 hover:text-blue-500 hover:scale-105 hover:shadow-lg transition-transform duration-200"

	return (
		<>
			<div className={categoryStyle} onClick={inject}>
				<p className="m-1 text-xs" >
					{name.replace(/_/g, ' ').toUpperCase()} 
				</p>
				<p> {testsInfo.passed} / {testsInfo.total} </p>
			</div>
		</>
	);
}

export default Category;
