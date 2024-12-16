import React, {useState} from 'react';
import UnitTest from './UnitTest';
import Resume from './Resume';

function Category({ name, inject }) {
	return (
		<>
			<div className="text-center border-2 border-zinc-300 py-3 category">
				<button onClick={inject}>
					<a>{name}</a>
				</button>
			</div>
		</>
	);
}

export default Category;
