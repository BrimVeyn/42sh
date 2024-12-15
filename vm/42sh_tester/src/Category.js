import React, {useState} from 'react';
import UnitTest from './UnitTest';
import "./Category.css"

function Category({name, tests}) {

	const [showTests, setShowTests] = useState(false);

	const toggleTests = () => {
		setShowTests((prev) => !prev);
	};

	return (
		<div className="category">
			<h2>Category: {name}</h2>
			<button onClick={toggleTests}>
				{showTests ? 'Hide' : 'Show'}
			</button>
			{showTests && (
				<ul>
					{tests.map((test, index) => (
						<li className="testLi" key={index}>
							<UnitTest unit={test} />
						</li>
					))}
				</ul>
			)}
		</div>
	);
}

export default Category;
